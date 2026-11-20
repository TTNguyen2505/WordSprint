#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaObject>
#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QThread>

DatabaseManager::DatabaseManager() : QObject(QCoreApplication::instance()) {}

DatabaseManager &DatabaseManager::instance() {
    static DatabaseManager *singleton = new DatabaseManager;
    return *singleton;
}

DatabaseManager::~DatabaseManager() {
    QMutexLocker locker(&m_mutex);
    closeDatabase();
}

QVariantMap DatabaseManager::initialize() {
    if (QThread::currentThread() != thread()) {
        QVariantMap result;
        const bool invoked = QMetaObject::invokeMethod(
                this,
                [this, &result]() {
                    result = initializeOnOwnerThread();
                },
                Qt::BlockingQueuedConnection);
        if (!invoked) {
            return failure(QStringLiteral("Không thể chuyển yêu cầu khởi tạo SQLite sang thread sở hữu."));
        }
        return result;
    }

    return initializeOnOwnerThread();
}

QVariantMap DatabaseManager::initializeOnOwnerThread() {
    Q_ASSERT(QThread::currentThread() == thread());
    QMutexLocker locker(&m_mutex);

    if (m_initialized) {
        return {{QStringLiteral("success"), true}};
    }

    const QString dataDirectory =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDirectory.isEmpty() || !QDir().mkpath(dataDirectory)) {
        return failure(QStringLiteral("Không thể tạo thư mục dữ liệu ứng dụng."));
    }

    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
        return failure(QStringLiteral("Không tìm thấy trình điều khiển SQLite của Qt."));
    }

    const QString databasePath = QDir(dataDirectory).filePath(QStringLiteral("wordsprint.sqlite3"));
    if (!QFileInfo::exists(databasePath)) {
        const QString seedPath = QStringLiteral(":/database/wordsprint.sqlite3");
        if (!QFile::exists(seedPath)) {
            return failure(QStringLiteral("Không tìm thấy SQLite seed trong tài nguyên ứng dụng."));
        }

        if (!QFile::copy(seedPath, databasePath) && !QFileInfo::exists(databasePath)) {
            return failure(QStringLiteral("Không thể sao chép SQLite seed vào thư mục dữ liệu ứng dụng."));
        }
    }

    if (QSqlDatabase::contains(m_connectionName)) {
        return failure(QStringLiteral("Kết nối SQLite wordsprint đã được đăng ký ngoài DatabaseManager."));
    }

    m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    m_database.setDatabaseName(databasePath);

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

QSqlDatabase DatabaseManager::database() const {
    if (QThread::currentThread() != thread()) {
        qWarning() << "SQLite connection requested outside the DatabaseManager thread.";
        return {};
    }

    QMutexLocker locker(&m_mutex);
    if (!m_initialized) {
        qWarning() << "SQLite connection requested before initialization.";
        return {};
    }

    return m_database;
}

QVariantMap DatabaseManager::failure(const QString &message) const {
    return {{QStringLiteral("success"), false}, {QStringLiteral("error"), message}};
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
