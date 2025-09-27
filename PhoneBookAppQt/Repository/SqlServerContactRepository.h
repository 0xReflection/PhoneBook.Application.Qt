#pragma once

#include "IRepository.h"
#include "infrastructure/DatabaseConnection.h"

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <stdexcept>
#include <sql.h>
#include <sqlext.h>

template<typename T>
class SqlServerContactRepository : public IRepository<T> {
private:
    std::unique_ptr<DatabaseConnection> m_dbConnection;

    class Statement final {
    private:
        SQLHSTMT m_stmt{ nullptr };

    public:
        explicit Statement(SQLHDBC connection) {
            m_stmt = nullptr;
            if (connection != nullptr) {
                SQLAllocHandle(SQL_HANDLE_STMT, connection, &m_stmt);
            }
        }

        ~Statement() noexcept {
            if (m_stmt) {
                SQLFreeHandle(SQL_HANDLE_STMT, m_stmt);
            }
        }

        Statement(const Statement&) = delete;
        Statement& operator=(const Statement&) = delete;

        Statement(Statement&& other) noexcept : m_stmt(std::exchange(other.m_stmt, nullptr)) {}
        Statement& operator=(Statement&& other) noexcept {
            if (this != &other) {
                if (m_stmt) SQLFreeHandle(SQL_HANDLE_STMT, m_stmt);
                m_stmt = std::exchange(other.m_stmt, nullptr);
            }
            return *this;
        }

        operator SQLHSTMT() const noexcept { return m_stmt; }
        bool isValid() const noexcept { return m_stmt != nullptr; }
    };

public:
    explicit SqlServerContactRepository(std::unique_ptr<DatabaseConnection> dbConn)
        : m_dbConnection(std::move(dbConn)) {
        if (!m_dbConnection) {
            throw std::invalid_argument("DatabaseConnection is null");
        }
        if (!m_dbConnection->isConnected()) {
            throw std::runtime_error("DatabaseConnection is not connected");
        }
    }

    bool add(const T& contact) override {
        if (!m_dbConnection) return false;
        if (!contact.isValid() || exists(contact.getPhoneNumber())) {
            return false;
        }

        Statement stmt(m_dbConnection->getHandle());
        if (!stmt.isValid()) return false;

        const std::wstring query = L"INSERT INTO Contacts (PhoneNumber, FullName) VALUES (?, ?)";

        SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            return false;
        }

        uint64_t phoneNumber = contact.getPhoneNumber();
        std::wstring fullName(contact.getFullName().begin(), contact.getFullName().end());

    
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_BIGINT,
            0, 0, (SQLPOINTER)&phoneNumber, 0, nullptr);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            static_cast<SQLLEN>(fullName.size()), 0,
            (SQLPOINTER)fullName.c_str(),
            static_cast<SQLINTEGER>(fullName.size() * sizeof(wchar_t)),
            nullptr);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        ret = SQLExecute(stmt);
        return ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO;
    }

    bool remove(uint64_t phoneNumber) override {
        if (!m_dbConnection) return false;

        Statement stmt(m_dbConnection->getHandle());
        if (!stmt.isValid()) return false;

        const std::wstring query = L"DELETE FROM Contacts WHERE PhoneNumber = ?";
        SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_BIGINT,
            0, 0, (SQLPOINTER)&phoneNumber, 0, nullptr);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        ret = SQLExecute(stmt);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        SQLLEN affectedRows = 0;
        SQLRowCount(stmt, &affectedRows);
        return affectedRows > 0;
    }

    std::unique_ptr<T> findByPhoneNumber(uint64_t phoneNumber) const override {
        if (!m_dbConnection) return nullptr;

        Statement stmt(m_dbConnection->getHandle());
        if (!stmt.isValid()) return nullptr;

        const std::wstring query = L"SELECT FullName FROM Contacts WHERE PhoneNumber = ?";
        SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return nullptr;

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_BIGINT,
            0, 0, (SQLPOINTER)&phoneNumber, 0, nullptr);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return nullptr;

        ret = SQLExecute(stmt);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return nullptr;

        ret = SQLFetch(stmt);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return nullptr;

        wchar_t fullName[512]{};
        SQLLEN indicator = 0;
        SQLGetData(stmt, 1, SQL_C_WCHAR, fullName, sizeof(fullName), &indicator);

        if (indicator <= 0) return nullptr;

        std::wstring wstr(fullName);
        std::string fullNameStr(wstr.begin(), wstr.end());

        return T::create(phoneNumber, fullNameStr);
    }

    std::vector<std::unique_ptr<T>> findAll() const override {
        std::vector<std::unique_ptr<T>> contacts;
        if (!m_dbConnection) return contacts;

        Statement stmt(m_dbConnection->getHandle());
        if (!stmt.isValid()) return contacts;

        const std::wstring query = L"SELECT PhoneNumber, FullName FROM Contacts ORDER BY FullName";
        SQLRETURN ret = SQLExecDirectW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return contacts;

        while (true) {
            ret = SQLFetch(stmt);
            if (ret == SQL_NO_DATA) break;
            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) break;

            uint64_t phoneNumber = 0;
            wchar_t fullName[512]{};
            SQLLEN ind1 = 0, ind2 = 0;

            SQLGetData(stmt, 1, SQL_C_UBIGINT, &phoneNumber, sizeof(phoneNumber), &ind1);
            SQLGetData(stmt, 2, SQL_C_WCHAR, fullName, sizeof(fullName), &ind2);

            if (ind2 > 0) {
                std::wstring wstr(fullName);
                std::string fullNameStr(wstr.begin(), wstr.end());
                if (auto contact = T::create(phoneNumber, fullNameStr)) {
                    contacts.push_back(std::move(contact));
                }
            }
        }

        return contacts;
    }

    bool exists(uint64_t phoneNumber) const override {
        if (!m_dbConnection) return false;

        Statement stmt(m_dbConnection->getHandle());
        if (!stmt.isValid()) return false;

        const std::wstring query = L"SELECT 1 FROM Contacts WHERE PhoneNumber = ?";
        SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_BIGINT,
            0, 0, (SQLPOINTER)&phoneNumber, 0, nullptr);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        ret = SQLExecute(stmt);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return false;

        ret = SQLFetch(stmt);
        return ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO;
    }

    size_t getCount() const override {
        if (!m_dbConnection) return 0;

        Statement stmt(m_dbConnection->getHandle());
        if (!stmt.isValid()) return 0;

        const std::wstring query = L"SELECT COUNT(*) FROM Contacts";
        SQLRETURN ret = SQLExecDirectW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return 0;

        ret = SQLFetch(stmt);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) return 0;

        SQLLEN count = 0;
        SQLGetData(stmt, 1, SQL_C_SLONG, &count, sizeof(count), nullptr);
        return static_cast<size_t>(count);
    }
};
