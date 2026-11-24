#include "DatabaseManager.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

#include <atomic>
#include <vector>

namespace {

std::atomic<unsigned long long> nextConnectionId{0};

std::vector<QString> splitSqlStatements(const QString& script) {
    std::vector<QString> statements;
    QString current;
    bool singleQuoted = false;
    bool doubleQuoted = false;
    bool backtickQuoted = false;
    bool bracketQuoted = false;
    bool lineComment = false;
    bool blockComment = false;

    for (qsizetype i = 0; i < script.size(); ++i) {
        const QChar character = script.at(i);
        const QChar next = (i + 1 < script.size()) ? script.at(i + 1) : QChar();

        if (lineComment) {
            if (character == QLatin1Char('\n') || character == QLatin1Char('\r')) {
                lineComment = false;
                current += character;
            }
            continue;
        }

        if (blockComment) {
            if (character == QLatin1Char('*') && next == QLatin1Char('/')) {
                blockComment = false;
                ++i;
            }
            continue;
        }

        if (singleQuoted || doubleQuoted || backtickQuoted) {
            current += character;
            const QChar delimiter = singleQuoted ? QLatin1Char('\'')
                : doubleQuoted ? QLatin1Char('"')
                               : QLatin1Char('`');
            if (character == delimiter) {
                if (next == delimiter) {
                    current += next;
                    ++i;
                } else if (singleQuoted) {
                    singleQuoted = false;
                } else if (doubleQuoted) {
                    doubleQuoted = false;
                } else {
                    backtickQuoted = false;
                }
            }
            continue;
        }

        if (bracketQuoted) {
            current += character;
            if (character == QLatin1Char(']')) {
                if (next == QLatin1Char(']')) {
                    current += next;
                    ++i;
                } else {
                    bracketQuoted = false;
                }
            }
            continue;
        }

        if (character == QLatin1Char('-') && next == QLatin1Char('-')) {
            lineComment = true;
            ++i;
            continue;
        }
        if (character == QLatin1Char('/') && next == QLatin1Char('*')) {
            blockComment = true;
            ++i;
            continue;
        }
        if (character == QLatin1Char('\'')) {
            singleQuoted = true;
            current += character;
            continue;
        }
        if (character == QLatin1Char('"')) {
            doubleQuoted = true;
            current += character;
            continue;
        }
        if (character == QLatin1Char('`')) {
            backtickQuoted = true;
            current += character;
            continue;
        }
        if (character == QLatin1Char('[')) {
            bracketQuoted = true;
            current += character;
            continue;
        }
        if (character == QLatin1Char(';')) {
            if (!current.trimmed().isEmpty()) {
                statements.push_back(current.trimmed());
            }
            current.clear();
            continue;
        }

        current += character;
    }

    if (!current.trimmed().isEmpty()) {
        statements.push_back(current.trimmed());
    }
    return statements;
}

}  // namespace

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

bool DatabaseManager::openDatabase(const std::string& databasePath) {
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

bool DatabaseManager::isConnected() const {
    return m_database.isValid() && m_database.isOpen();
}

bool DatabaseManager::initializeDatabase(const QString& schemaResourcePath,
                                         const QString& seedResourcePath) {
    if (!isConnected()) {
        setLastError(QStringLiteral("The database connection is not open."));
        return false;
    }

    if (!m_database.transaction()) {
        setLastError(m_database.lastError().text());
        return false;
    }

    if (!executeSqlResource(schemaResourcePath) || !executeSqlResource(seedResourcePath)) {
        m_database.rollback();
        return false;
    }

    if (!m_database.commit()) {
        setLastError(m_database.lastError().text());
        m_database.rollback();
        return false;
    }

    m_lastError.clear();
    return true;
}

QSqlDatabase DatabaseManager::connection() const {
    return m_database;
}

const std::string& DatabaseManager::lastError() const noexcept {
    return m_lastError;
}

bool DatabaseManager::executeSqlResource(const QString& resourcePath) {
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        setLastError(QStringLiteral("Unable to open SQL resource: %1").arg(resourcePath));
        return false;
    }

    QByteArray script = file.readAll();
    if (script.size() >= 3 &&
        static_cast<unsigned char>(script[0]) == 0xEF &&
        static_cast<unsigned char>(script[1]) == 0xBB &&
        static_cast<unsigned char>(script[2]) == 0xBF) {
        script.remove(0, 3);
    }

    const QString sqlScript = QString::fromUtf8(script.constData(), script.size());
    for (const QString& statement : splitSqlStatements(sqlScript)) {
        QSqlQuery query(m_database);
        if (!query.exec(statement)) {
            setLastError(query.lastError().text());
            return false;
        }
    }

    return true;
}

void DatabaseManager::setLastError(const QString& error) {
    m_lastError = error.toStdString();
}
