// DatabaseManager.cpp
// Implementation of DatabaseManager – thin RAII wrapper around Qt's QSqlDatabase.

#include "DatabaseManager.h"
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString &dbPath) {
    // Use the built‑in QSQLITE driver.
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), "WordSprintConnection");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qCritical() << "Failed to open SQLite database:" << m_db.lastError().text();
    } else {
        // Enable foreign‑key constraints.
        QSqlQuery pragma(m_db);
        pragma.exec("PRAGMA foreign_keys = ON;");
        if (pragma.lastError().isValid()) {
            qWarning() << "Could not set PRAGMA foreign_keys:" << pragma.lastError().text();
        }
    }
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    // Remove the connection from the QSqlDatabase pool.
    QSqlDatabase::removeDatabase("WordSprintConnection");
}

QSqlDatabase &DatabaseManager::db() {
    return m_db;
}

bool DatabaseManager::exec(const QSqlQuery &query, QString *error) {
    if (query.lastError().isValid()) {
        if (error) *error = query.lastError().text();
        qWarning() << "SQL exec error:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQuery DatabaseManager::prepare(const QString &sql, QString *error) {
    QSqlQuery q(m_db);
    if (!q.prepare(sql)) {
        if (error) *error = q.lastError().text();
        qWarning() << "Failed to prepare query:" << sql << "error:" << q.lastError().text();
    }
    return q;
}
