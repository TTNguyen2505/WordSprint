#include "UserRepository.h"

#include "SqliteRepositoryUtils.h"
#include "core/DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

UserRepository::UserRepository(DatabaseManager &databaseManager) : m_databaseManager(databaseManager) {}

std::optional<std::vector<User>> UserRepository::getAllUsers() {
    std::vector<User> users;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "UserRepository", "getAllUsers")) {
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.exec(QStringLiteral("SELECT user_id, username, created_at, bgm_enabled, sfx_enabled, "
                                   "bgm_volume, sfx_volume FROM users ORDER BY user_id"))) {
        SqliteRepositoryUtils::warnQueryFailure("UserRepository", "getAllUsers", query);
        return std::nullopt;
    }

    while (query.next()) {
        users.push_back(SqliteRepositoryUtils::mapUser(query));
    }
    return users;
}

std::optional<User> UserRepository::createUser(const std::string &username) {
    const QString normalizedUsername = QString::fromStdString(username).trimmed();
    if (normalizedUsername.isEmpty()) {
        qWarning() << "UserRepository::createUser received an empty username.";
        return std::nullopt;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "UserRepository", "createUser")) {
        return std::nullopt;
    }

    QSqlQuery insertQuery(database);
    if (!insertQuery.prepare(QStringLiteral("INSERT INTO users (username) VALUES (?)"))) {
        SqliteRepositoryUtils::warnQueryFailure("UserRepository", "createUser.prepare", insertQuery);
        return std::nullopt;
    }
    insertQuery.addBindValue(normalizedUsername);
    if (!insertQuery.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("UserRepository", "createUser", insertQuery);
        return std::nullopt;
    }

    bool idOk = false;
    const int userId = insertQuery.lastInsertId().toInt(&idOk);
    if (!idOk) {
        qWarning() << "UserRepository::createUser succeeded, but SQLite did not return user_id.";
        return std::nullopt;
    }

    QSqlQuery selectQuery(database);
    if (!selectQuery.prepare(QStringLiteral("SELECT user_id, username, created_at, bgm_enabled, sfx_enabled, "
                                            "bgm_volume, sfx_volume FROM users WHERE user_id = ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("UserRepository", "createUser.select.prepare", selectQuery);
        return std::nullopt;
    }
    selectQuery.addBindValue(userId);
    if (!selectQuery.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("UserRepository", "createUser.select", selectQuery);
        return std::nullopt;
    }
    if (!selectQuery.next()) {
        qWarning() << "UserRepository::createUser inserted a user but could not read it back.";
        return std::nullopt;
    }
    return SqliteRepositoryUtils::mapUser(selectQuery);
}

bool UserRepository::updateAudioSettings(int userId, bool bgmOn, bool sfxOn, int bgmVol, int sfxVol) {
    if (userId <= 0 || bgmVol < 0 || bgmVol > 100 || sfxVol < 0 || sfxVol > 100) {
        qWarning() << "UserRepository::updateAudioSettings received invalid user or volume values.";
        return false;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "UserRepository", "updateAudioSettings")) {
        return false;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral("UPDATE users SET bgm_enabled = ?, sfx_enabled = ?, "
                                      "bgm_volume = ?, sfx_volume = ? WHERE user_id = ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("UserRepository", "updateAudioSettings.prepare", query);
        return false;
    }
    query.addBindValue(bgmOn);
    query.addBindValue(sfxOn);
    query.addBindValue(bgmVol);
    query.addBindValue(sfxVol);
    query.addBindValue(userId);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("UserRepository", "updateAudioSettings", query);
        return false;
    }
    if (query.numRowsAffected() == 0) {
        qWarning() << "UserRepository::updateAudioSettings found no user with id" << userId;
        return false;
    }
    return true;
}
