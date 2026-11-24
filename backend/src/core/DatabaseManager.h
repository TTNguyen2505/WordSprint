#pragma once

#include <QSqlDatabase>
#include <QString>

#include <string>

class DatabaseManager final {
public:
    DatabaseManager();
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;
    DatabaseManager(DatabaseManager &&) = delete;
    DatabaseManager &operator=(DatabaseManager &&) = delete;

    bool openDatabase(const std::string &databasePath);
    void closeDatabase();
    bool isConnected() const;
    bool initializeDatabase(const std::string &schemaPath, const std::string &appDataSeedPath,
                            const std::string &mockUsersSeedPath);

    QSqlDatabase connection() const;
    const std::string &lastError() const noexcept;

private:
    bool executeSqlFile(const std::string &filePath);
    void setLastError(const QString &error);

    QString m_connectionName;
    QSqlDatabase m_database;
    std::string m_lastError;
};
