#pragma once

#include <QSqlDatabase>
#include <QString>

#include <string>

class DatabaseManager final {
public:
    DatabaseManager();
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) = delete;
    DatabaseManager& operator=(DatabaseManager&&) = delete;

    bool openDatabase(const std::string& databasePath);
    void closeDatabase();
    bool isConnected() const;
    bool initializeDatabase(const QString& schemaResourcePath, const QString& seedResourcePath);

    QSqlDatabase connection() const;
    const std::string& lastError() const noexcept;

private:
    bool executeSqlResource(const QString& resourcePath);
    void setLastError(const QString& error);

    QString m_connectionName;
    QSqlDatabase m_database;
    std::string m_lastError;
};
