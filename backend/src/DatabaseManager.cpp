#include "DatabaseManager.h"

#include "DatabaseWorker.h"

#include <QMetaObject>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), worker_(new DatabaseWorker) {
    worker_->moveToThread(&workerThread_);
    connect(&workerThread_, &QThread::finished, worker_, &QObject::deleteLater);
    connect(worker_, &DatabaseWorker::initializationFinished, this,
            [this](bool success, const QString &errorMessage) {
                initializing_ = false;
                ready_ = success;
                emit readyChanged();
                emit initializationFinished(success, errorMessage);
            });
    workerThread_.start();
}

DatabaseManager::~DatabaseManager() {
    if (workerThread_.isRunning()) {
        QMetaObject::invokeMethod(worker_, "shutdown", Qt::BlockingQueuedConnection);
        workerThread_.quit();
        workerThread_.wait();
    }
}

bool DatabaseManager::ready() const {
    return ready_;
}

void DatabaseManager::initialize() {
    if (initializing_ || ready_) {
        return;
    }

    initializing_ = true;
    QMetaObject::invokeMethod(worker_, "initialize", Qt::QueuedConnection);
}
