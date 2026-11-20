#pragma once

#include <QMutex>
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVariantMap>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    static DatabaseManager &instance();
    ~DatabaseManager() override;

    Q_INVOKABLE QVariantMap initialize();
    QSqlDatabase database() const;

private:
    DatabaseManager();

    QVariantMap initializeOnOwnerThread();
    QVariantMap failure(const QString &message) const;
    void closeDatabase();

    mutable QMutex m_mutex;
    QSqlDatabase m_database;
    const QString m_connectionName = QStringLiteral("wordsprint");
    bool m_initialized = false;
};
