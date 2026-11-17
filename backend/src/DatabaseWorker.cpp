#include "DatabaseWorker.h"

#include <QDir>
#include <QSqlError>
#include <QStandardPaths>
#include <QUuid>

void DatabaseWorker::initialize() {
    if (database_.isOpen()) {
        emit initializationFinished(true, QString());
        return;
    }

    const QString dataDirectory =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDirectory.isEmpty() || !QDir().mkpath(dataDirectory)) {
        emit initializationFinished(
                false, QStringLiteral("Không thể tạo thư mục dữ liệu ứng dụng."));
        return;
    }

    connectionName_ = QUuid::createUuid().toString(QUuid::WithoutBraces);
    database_ = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName_);
    database_.setDatabaseName(QDir(dataDirectory).filePath(QStringLiteral("wordsprint.sqlite")));
    if (!database_.open()) {
        const QString errorMessage = database_.lastError().text();
        closeConnection();
        emit initializationFinished(false, errorMessage);
        return;
    }

    emit initializationFinished(true, QString());
}

void DatabaseWorker::shutdown() {
    closeConnection();
}

void DatabaseWorker::closeConnection() {
    if (database_.isValid()) {
        database_.close();
    }
    database_ = QSqlDatabase();
    if (!connectionName_.isEmpty()) {
        QSqlDatabase::removeDatabase(connectionName_);
        connectionName_.clear();
    }
}
