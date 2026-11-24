#include "RecordRepository.h"

#include "SqliteRepositoryUtils.h"
#include "core/DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

RecordRepository::RecordRepository(DatabaseManager &databaseManager) : m_databaseManager(databaseManager) {}

std::optional<StageRecord> RecordRepository::insertRecord(int userId, int stageId, int score, int completionTime) {
    if (userId <= 0 || stageId <= 0 || score < 0 || completionTime < 0) {
        qWarning() << "RecordRepository::insertRecord received invalid record values.";
        return std::nullopt;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "RecordRepository", "insertRecord")) {
        return std::nullopt;
    }

    QSqlQuery insertQuery(database);
    if (!insertQuery.prepare(QStringLiteral("INSERT INTO stage_records "
                                            "(user_id, stage_id, score, completion_time) VALUES (?, ?, ?, ?)"))) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "insertRecord.prepare", insertQuery);
        return std::nullopt;
    }
    insertQuery.addBindValue(userId);
    insertQuery.addBindValue(stageId);
    insertQuery.addBindValue(score);
    insertQuery.addBindValue(completionTime);
    if (!insertQuery.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "insertRecord", insertQuery);
        return std::nullopt;
    }

    bool idOk = false;
    const int recordId = insertQuery.lastInsertId().toInt(&idOk);
    if (!idOk) {
        qWarning() << "RecordRepository::insertRecord succeeded, but SQLite did not return record_id.";
        return std::nullopt;
    }

    QSqlQuery selectQuery(database);
    if (!selectQuery.prepare(QStringLiteral("SELECT record_id, user_id, stage_id, score, completion_time, played_at "
                                            "FROM stage_records WHERE record_id = ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "insertRecord.select.prepare", selectQuery);
        return std::nullopt;
    }
    selectQuery.addBindValue(recordId);
    if (!selectQuery.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "insertRecord.select", selectQuery);
        return std::nullopt;
    }
    if (!selectQuery.next()) {
        qWarning() << "RecordRepository::insertRecord inserted a record but could not read it back.";
        return std::nullopt;
    }
    return SqliteRepositoryUtils::mapStageRecord(selectQuery);
}

std::optional<StageRecord> RecordRepository::getBestRecord(int userId, int stageId) {
    if (userId <= 0 || stageId <= 0) {
        qWarning() << "RecordRepository::getBestRecord received invalid user or stage values.";
        return std::nullopt;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "RecordRepository", "getBestRecord")) {
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral("SELECT record_id, user_id, stage_id, score, completion_time, played_at "
                                      "FROM stage_records WHERE user_id = ? AND stage_id = ? "
                                      "ORDER BY score DESC, completion_time ASC, played_at ASC, record_id ASC "
                                      "LIMIT 1"))) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getBestRecord.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(userId);
    query.addBindValue(stageId);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getBestRecord", query);
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }
    return SqliteRepositoryUtils::mapStageRecord(query);
}

std::optional<std::vector<StageRecord>> RecordRepository::getBestRecords(int userId) {
    if (userId <= 0) {
        qWarning() << "RecordRepository::getBestRecords received an invalid user id.";
        return std::nullopt;
    }
    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "RecordRepository", "getBestRecords")) {
        return std::nullopt;
    }
    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
                "SELECT record_id, user_id, stage_id, score, completion_time, played_at FROM ("
                "SELECT *, ROW_NUMBER() OVER (PARTITION BY stage_id ORDER BY score DESC, completion_time ASC, "
                "played_at ASC, record_id ASC) AS attempt_rank FROM stage_records WHERE user_id = ?"
                ") WHERE attempt_rank = 1"))) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getBestRecords.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(userId);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getBestRecords", query);
        return std::nullopt;
    }
    std::vector<StageRecord> records;
    while (query.next()) {
        records.push_back(SqliteRepositoryUtils::mapStageRecord(query));
    }
    return records;
}

std::optional<std::vector<StageRecordDTO>> RecordRepository::getStageLeaderboard(int stageId, int limit) {
    std::vector<StageRecordDTO> records;
    if (stageId <= 0 || limit <= 0) {
        qWarning() << "RecordRepository::getStageLeaderboard received invalid stage or limit values.";
        return std::nullopt;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "RecordRepository", "getStageLeaderboard")) {
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral("SELECT r.record_id, r.user_id, r.stage_id, r.score, r.completion_time, "
                                      "r.played_at, u.username FROM stage_records r "
                                      "JOIN users u ON u.user_id = r.user_id WHERE r.stage_id = ? "
                                      "ORDER BY r.score DESC, r.completion_time ASC, r.played_at ASC, "
                                      "r.record_id ASC LIMIT ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getStageLeaderboard.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(stageId);
    query.addBindValue(limit);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getStageLeaderboard", query);
        return std::nullopt;
    }

    while (query.next()) {
        StageRecordDTO record;
        record.record = SqliteRepositoryUtils::mapStageRecord(query);
        record.username = query.value(6).toString().toStdString();
        record.rank = static_cast<int>(records.size()) + 1;
        records.push_back(std::move(record));
    }
    return records;
}

std::optional<std::vector<OverallRankDTO>> RecordRepository::getOverallLeaderboard(int limit) {
    std::vector<OverallRankDTO> ranks;
    if (limit <= 0) {
        qWarning() << "RecordRepository::getOverallLeaderboard received an invalid limit.";
        return std::nullopt;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "RecordRepository", "getOverallLeaderboard")) {
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
                "WITH ranked_records AS ("
                "    SELECT user_id, stage_id, score, "
                "           ROW_NUMBER() OVER (PARTITION BY user_id, stage_id "
                "                              ORDER BY score DESC, completion_time ASC, "
                "                                       played_at ASC, record_id ASC) AS attempt_rank "
                "    FROM stage_records"
                "), best_per_stage AS ("
                "    SELECT user_id, stage_id, score FROM ranked_records WHERE attempt_rank = 1"
                ") "
                "SELECT u.user_id, u.username, SUM(b.score) AS total_score, COUNT(*) AS completed_stages "
                "FROM best_per_stage b JOIN users u ON u.user_id = b.user_id "
                "GROUP BY u.user_id, u.username "
                "ORDER BY total_score DESC, completed_stages DESC, u.username COLLATE NOCASE ASC, u.user_id ASC "
                "LIMIT ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getOverallLeaderboard.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(limit);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("RecordRepository", "getOverallLeaderboard", query);
        return std::nullopt;
    }

    while (query.next()) {
        OverallRankDTO rank;
        rank.user_id = query.value(0).toInt();
        rank.username = query.value(1).toString().toStdString();
        rank.total_score = query.value(2).toLongLong();
        rank.completed_stages = query.value(3).toInt();
        rank.rank = static_cast<int>(ranks.size()) + 1;
        ranks.push_back(std::move(rank));
    }
    return ranks;
}
