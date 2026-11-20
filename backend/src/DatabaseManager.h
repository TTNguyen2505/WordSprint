#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QVariantMap>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    Q_INVOKABLE QVariantMap initialize();

private:
    void closeDatabase();

    QSqlDatabase m_database;
    QString m_connectionName = QStringLiteral("wordsprint");
    bool m_initialized = false;
};
