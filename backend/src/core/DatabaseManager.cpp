#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>

#ifdef _WIN32
#include <winsqlite/winsqlite3.h>
#else
#include <sqlite3.h>
#endif

#include <atomic>
#include <fstream>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace {
    std::atomic<unsigned long long> nextConnectionId{0};
    constexpr int databaseVersion = 1;
} // namespace

DatabaseManager::DatabaseManager() {
    const auto connectionId = nextConnectionId.fetch_add(1);
    m_connectionName = QStringLiteral("WordSprintDatabase_%1").arg(static_cast<qulonglong>(connectionId));
    m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
    m_database = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool DatabaseManager::openDatabase(const std::string &databasePath) {
    if (!m_database.isValid()) {
        setLastError(QStringLiteral("The QSQLITE driver is unavailable."));
        return false;
    }

    if (m_database.isOpen()) {
        m_database.close();
    }

    m_database.setDatabaseName(QString::fromStdString(databasePath));
    if (!m_database.open()) {
        setLastError(m_database.lastError().text());
        return false;
    }

    QSqlQuery query(m_database);
    if (!query.exec(QStringLiteral("PRAGMA foreign_keys = ON;"))) {
        setLastError(query.lastError().text());
        m_database.close();
        return false;
    }

    m_lastError.clear();
    return true;
}

void DatabaseManager::closeDatabase() {
    if (m_database.isValid() && m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::isConnected() const { return m_database.isValid() && m_database.isOpen(); }

bool DatabaseManager::initializeDatabase(const std::string &schemaPath, const std::string &appDataSeedPath,
                                         const std::string &mockUsersSeedPath) {
    if (!isConnected()) {
        setLastError(QStringLiteral("The database connection is not open."));
        return false;
    }
    m_lastError.clear();

    QSqlQuery foreignKeys(m_database);
    if (!foreignKeys.exec(QStringLiteral("PRAGMA foreign_keys = OFF"))) {
        setLastError(foreignKeys.lastError().text());
        return false;
    }

    const auto restoreForeignKeys = [this]() {
        QSqlQuery query(m_database);
        if (!query.exec(QStringLiteral("PRAGMA foreign_keys = ON")) ||
            !query.exec(QStringLiteral("PRAGMA foreign_keys")) || !query.next() || !query.value(0).toBool()) {
            const QString error = query.lastError().isValid() ? query.lastError().text()
                                                              : QStringLiteral("Could not enable foreign keys.");
            const QString previous = QString::fromStdString(m_lastError);
            setLastError(previous.isEmpty() ? error : previous + QStringLiteral("; ") + error);
            return false;
        }
        return true;
    };
    if (!m_database.transaction()) {
        setLastError(m_database.lastError().text());
        restoreForeignKeys();
        return false;
    }

    const auto failTransaction = [this, &restoreForeignKeys]() {
        if (!m_database.rollback()) {
            setLastError(QString::fromStdString(m_lastError) + QStringLiteral("; rollback failed: ") +
                         m_database.lastError().text());
        }
        restoreForeignKeys();
        return false;
    };

    QSqlQuery versionQuery(m_database);
    if (!versionQuery.exec(QStringLiteral("PRAGMA user_version")) || !versionQuery.next()) {
        setLastError(versionQuery.lastError().isValid() ? versionQuery.lastError().text()
                                                        : QStringLiteral("Could not read the database version."));
        return failTransaction();
    }
    const int version = versionQuery.value(0).toInt();
    versionQuery.finish();
    if (version > databaseVersion) {
        setLastError(QStringLiteral("The database was created by a newer version of WordSprint."));
        return failTransaction();
    }

    if (version < databaseVersion) {
        QSqlQuery objects(m_database);
        if (!objects.exec(QStringLiteral("SELECT type, name FROM sqlite_master "
                                         "WHERE type IN ('trigger', 'view', 'table') AND name NOT GLOB 'sqlite_*' "
                                         "ORDER BY CASE type WHEN 'trigger' THEN 0 WHEN 'view' THEN 1 ELSE 2 END"))) {
            setLastError(objects.lastError().text());
            return failTransaction();
        }
        std::vector<std::pair<QString, QString>> objectsToDrop;
        while (objects.next()) {
            objectsToDrop.emplace_back(objects.value(0).toString(), objects.value(1).toString());
        }
        if (objects.lastError().isValid()) {
            setLastError(objects.lastError().text());
            return failTransaction();
        }
        objects.finish();
        for (auto &[type, name]: objectsToDrop) {
            name.replace(QLatin1Char('"'), QStringLiteral("\"\""));
            QSqlQuery drop(m_database);
            if (!drop.exec(QStringLiteral("DROP %1 IF EXISTS \"%2\"").arg(type.toUpper(), name))) {
                setLastError(drop.lastError().text());
                return failTransaction();
            }
        }

        if (!executeSqlFile(schemaPath) || !executeSqlFile(appDataSeedPath) || !executeSqlFile(mockUsersSeedPath)) {
            return failTransaction();
        }
        QSqlQuery check(m_database);
        if (!check.exec(QStringLiteral("PRAGMA foreign_key_check"))) {
            setLastError(check.lastError().text());
            return failTransaction();
        }
        if (check.next()) {
            setLastError(QStringLiteral("The new database contains invalid foreign keys in table %1.")
                                 .arg(check.value(0).toString()));
            return failTransaction();
        }
        if (check.lastError().isValid()) {
            setLastError(check.lastError().text());
            return failTransaction();
        }
        check.finish();
        QSqlQuery setVersion(m_database);
        if (!setVersion.exec(QStringLiteral("PRAGMA user_version = %1").arg(databaseVersion))) {
            setLastError(setVersion.lastError().text());
            return failTransaction();
        }
    }

    if (!m_database.commit()) {
        setLastError(m_database.lastError().text());
        return failTransaction();
    }
    m_lastError.clear();
    if (!restoreForeignKeys()) {
        return false;
    }
    return true;
}

QSqlDatabase DatabaseManager::connection() const { return m_database; }

const std::string &DatabaseManager::lastError() const noexcept { return m_lastError; }

bool DatabaseManager::executeSqlFile(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        setLastError(QStringLiteral("Unable to open SQL file: %1").arg(QString::fromStdString(filePath)));
        return false;
    }

    std::string script((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (file.bad()) {
        setLastError(QStringLiteral("Unable to read SQL file: %1").arg(QString::fromStdString(filePath)));
        return false;
    }
    if (script.size() >= 3 && static_cast<unsigned char>(script[0]) == 0xEF &&
        static_cast<unsigned char>(script[1]) == 0xBB && static_cast<unsigned char>(script[2]) == 0xBF) {
        script.erase(0, 3);
    }
    if (script.find('\0') != std::string::npos) {
        setLastError(QStringLiteral("SQL file contains a NUL byte: %1").arg(QString::fromStdString(filePath)));
        return false;
    }

    sqlite3 *parserHandle = nullptr;
    const int openResult = sqlite3_open(":memory:", &parserHandle);
    const std::unique_ptr<sqlite3, decltype(&sqlite3_close)> parser(parserHandle, &sqlite3_close);
    if (openResult != SQLITE_OK) {
        setLastError(QStringLiteral("Unable to initialize the SQL parser: %1")
                             .arg(parser ? QString::fromUtf8(sqlite3_errmsg(parser.get()))
                                         : QStringLiteral("out of memory")));
        return false;
    }

    const auto executeStatement = [this, &parser](const std::string &statement) {
        sqlite3_stmt *rawStatement = nullptr;
        const int prepareResult = sqlite3_prepare_v2(parser.get(), statement.c_str(), -1, &rawStatement, nullptr);
        const std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> parsedStatement(rawStatement,
                                                                                         &sqlite3_finalize);
        if (prepareResult == SQLITE_NOMEM) {
            setLastError(QStringLiteral("Out of memory while parsing SQL."));
            return false;
        }
        if (prepareResult == SQLITE_OK && !parsedStatement) {
            return true;
        }

        QSqlQuery query(m_database);
        if (!query.exec(QString::fromUtf8(statement.data(), static_cast<qsizetype>(statement.size())))) {
            setLastError(query.lastError().text());
            return false;
        }
        return true;
    };

    std::string statement;
    for (const char character: script) {
        statement += character;
        if (character != ';') {
            continue;
        }
        const int complete = sqlite3_complete(statement.c_str());
        if (complete != 0 && complete != 1) {
            setLastError(QStringLiteral("Out of memory while parsing SQL."));
            return false;
        }
        if (complete == 1) {
            if (!executeStatement(statement)) {
                return false;
            }
            statement.clear();
        }
    }

    if (!statement.empty()) {
        const std::string terminated = statement + "\n;";
        const int complete = sqlite3_complete(terminated.c_str());
        if (complete != 1) {
            setLastError(complete == 0 ? QStringLiteral("Incomplete SQL statement in file: %1")
                                                 .arg(QString::fromStdString(filePath))
                                       : QStringLiteral("Out of memory while parsing SQL."));
            return false;
        }
        if (!executeStatement(statement)) {
            return false;
        }
    }

    return true;
}

void DatabaseManager::setLastError(const QString &error) { m_lastError = error.toStdString(); }
