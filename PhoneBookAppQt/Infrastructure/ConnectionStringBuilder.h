#pragma once
#include <string>

class ConnectionStringBuilder {
public:
    static std::wstring buildForSqlServer(
        const std::wstring& server = L"HOME-PC\\SQLEXPRESS",
        const std::wstring& database = L"PhoneBookDB",
        const std::wstring& username = L"",
        const std::wstring& password = L"",
        bool trustedConnection = true)
    {
        if (trustedConnection) {
            return L"DRIVER={ODBC Driver 17 for SQL Server};SERVER=" + server +
                L";DATABASE=" + database + L";Trusted_Connection=yes;";
        }
        else {
            return L"DRIVER={ODBC Driver 17 for SQL Server};SERVER=" + server +
                L";DATABASE=" + database + L";UID=" + username +
                L";PWD=" + password + L";";
        }
    }
};
