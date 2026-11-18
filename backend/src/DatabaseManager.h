// DatabaseManager.h
// Simple wrapper around Qt's QSqlDatabase for the backend.
// Provides RAII handling and a single shared connection.

#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QString>
#include <memory>

class DatabaseManager {
public:
    // Construct with the path to the SQLite file.
    explicit DatabaseManager(const QString &dbPath);
    ~DatabaseManager();

    // Disable copy / move semantics – use a singleton or shared_ptr.
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    // Access the underlying QSqlDatabase.
    QSqlDatabase &db();

    // Execute a query that does not return rows (INSERT/UPDATE/DELETE).
    bool exec(const QSqlQuery &query, QString *error = nullptr);

    // Helper to prepare a query with error reporting.
    QSqlQuery prepare(const QString &sql, QString *error = nullptr);

private:
    QSqlDatabase m_db;
};
