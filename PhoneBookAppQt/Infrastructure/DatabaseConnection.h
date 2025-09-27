#pragma once
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <memory>
#include <stdexcept>

class DatabaseConnection {
private:
    SQLHENV m_environment{ nullptr };
    SQLHDBC m_connection{ nullptr };
    bool m_connected{ false };

    void cleanup() noexcept {
        if (m_connection) { SQLDisconnect(m_connection); SQLFreeHandle(SQL_HANDLE_DBC, m_connection); }
        if (m_environment) { SQLFreeHandle(SQL_HANDLE_ENV, m_environment); }
        m_connected = false;
    }

public:
    explicit DatabaseConnection(const std::wstring& connectionString) {
        if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_environment) != SQL_SUCCESS)
            throw std::runtime_error("Failed to allocate environment");
        SQLSetEnvAttr(m_environment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

        if (SQLAllocHandle(SQL_HANDLE_DBC, m_environment, &m_connection) != SQL_SUCCESS) {
            cleanup();
            throw std::runtime_error("Failed to allocate connection handle");
        }

        SQLWCHAR retconstring[1024];
        SQLSMALLINT retconstringlen;
        SQLRETURN ret = SQLDriverConnectW(m_connection, nullptr,
            (SQLWCHAR*)connectionString.c_str(), SQL_NTS,
            retconstring, sizeof(retconstring), &retconstringlen, SQL_DRIVER_NOPROMPT);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            cleanup();
            throw std::runtime_error("Database connection failed");
        }

        m_connected = true;
    }

    ~DatabaseConnection() noexcept { cleanup(); }

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    SQLHDBC getHandle() const noexcept { return m_connection; }
    bool isConnected() const noexcept { return m_connected; }
};
