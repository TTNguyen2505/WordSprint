#include "DatabaseManager.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

QVariantMap DatabaseManager::initialize() {
    if (m_initialized) {
        return {{QStringLiteral("success"), true}};
    }

    if (QSqlDatabase::contains(m_connectionName)) {
        closeDatabase();
    }

    const QString dataDirectory =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDirectory.isEmpty() || !QDir().mkpath(dataDirectory)) {
        return {{QStringLiteral("success"), false},
                {QStringLiteral("error"),
                 QStringLiteral("Không thể tạo thư mục dữ liệu ứng dụng.")}};
    }

    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
        return {{QStringLiteral("success"), false},
                {QStringLiteral("error"),
                 QStringLiteral("Không tìm thấy trình điều khiển SQLite của Qt.")}};
    }

    m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    m_database.setDatabaseName(dataDirectory + QStringLiteral("/wordsprint.sqlite3"));

    if (!m_database.open()) {
        const QString error = m_database.lastError().text();
        closeDatabase();
        return {{QStringLiteral("success"), false}, {QStringLiteral("error"), error}};
    }

    QString integrityError;
    {
        QSqlQuery query(m_database);
        if (!query.exec(QStringLiteral("PRAGMA quick_check"))) {
            integrityError = query.lastError().text();
        } else if (!query.next() || query.value(0).toString() != QStringLiteral("ok")) {
            integrityError = query.value(0).toString();
            if (integrityError.isEmpty()) {
                integrityError = QStringLiteral("Kiểm tra toàn vẹn SQLite không thành công.");
            }
        }
    }

    if (!integrityError.isEmpty()) {
        closeDatabase();
        return {{QStringLiteral("success"), false}, {QStringLiteral("error"), integrityError}};
    }

    m_initialized = true;
    return {{QStringLiteral("success"), true}};
}

void DatabaseManager::closeDatabase() {
    if (!m_database.isValid()) {
        return;
    }

    m_database.close();
    m_database = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
    m_initialized = false;
}
