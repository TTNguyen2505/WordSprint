#pragma once

#include <QObject>
#include <QThread>
#include <QString>

class DatabaseWorker;

class DatabaseManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    bool ready() const;

    Q_INVOKABLE void initialize();

signals:
    void readyChanged();
    void initializationFinished(bool success, const QString &errorMessage);

private:
    DatabaseWorker *worker_;
    QThread workerThread_;
    bool ready_ = false;
    bool initializing_ = false;
};
