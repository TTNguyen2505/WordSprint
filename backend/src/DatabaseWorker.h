#pragma once

#include <QObject>
#include <QString>
#include <QSqlDatabase>

class DatabaseWorker : public QObject {
    Q_OBJECT

public slots:
    void initialize();
    void shutdown();

signals:
    void initializationFinished(bool success, const QString &errorMessage);

private:
    void closeConnection();

    QSqlDatabase database_;
    QString connectionName_;
};
