#include "StageRepository.h"

#include "SqliteRepositoryUtils.h"
#include "core/DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

#include <string>
#include <utility>

StageRepository::StageRepository(DatabaseManager &databaseManager) : m_databaseManager(databaseManager) {}

std::optional<std::vector<Topic>> StageRepository::getAllTopics() {
    std::vector<Topic> topics;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "StageRepository", "getAllTopics")) {
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.exec(QStringLiteral("SELECT topic_id, topic_name FROM topics ORDER BY topic_id"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getAllTopics", query);
        return std::nullopt;
    }
    while (query.next()) {
        Topic topic;
        topic.id = query.value(0).toInt();
        topic.name = query.value(1).toString().toStdString();
        topics.push_back(std::move(topic));
    }
    return topics;
}

std::vector<Stage> StageRepository::getStagesByTopicId(int topicId) {
    std::vector<Stage> stages;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "StageRepository", "getStagesByTopicId")) {
        return stages;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral("SELECT stage_id, topic_id, cefr_level, time_limit, question_count, pass_score "
                                      "FROM stages WHERE topic_id = ? "
                                      "ORDER BY CASE cefr_level WHEN 'A1' THEN 1 WHEN 'A2' THEN 2 "
                                      "WHEN 'B1' THEN 3 WHEN 'B2' THEN 4 WHEN 'C1' THEN 5 WHEN 'C2' THEN 6 END"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStagesByTopicId.prepare", query);
        return stages;
    }
    query.addBindValue(topicId);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStagesByTopicId", query);
        return stages;
    }
    while (query.next()) {
        stages.push_back(SqliteRepositoryUtils::mapStage(query));
    }
    return stages;
}

std::optional<Stage> StageRepository::getStageById(int stageId) {
    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "StageRepository", "getStageById")) {
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral("SELECT stage_id, topic_id, cefr_level, time_limit, question_count, pass_score "
                                      "FROM stages WHERE stage_id = ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStageById.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(stageId);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStageById", query);
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }
    return SqliteRepositoryUtils::mapStage(query);
}

std::optional<std::vector<StageStatusDTO>> StageRepository::getStagesWithUnlockStatus(int userId) {
    std::vector<StageStatusDTO> results;
    if (userId <= 0) {
        qWarning() << "StageRepository::getStagesWithUnlockStatus received an invalid user id.";
        return std::nullopt;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "StageRepository", "getStagesWithUnlockStatus")) {
        return std::nullopt;
    }

    QSqlQuery userQuery(database);
    if (!userQuery.prepare(QStringLiteral("SELECT 1 FROM users WHERE user_id = ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStagesWithUnlockStatus.user.prepare", userQuery);
        return std::nullopt;
    }
    userQuery.addBindValue(userId);
    if (!userQuery.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStagesWithUnlockStatus.user", userQuery);
        return std::nullopt;
    }
    if (!userQuery.next()) {
        qWarning() << "StageRepository::getStagesWithUnlockStatus found no user with id" << userId;
        return std::nullopt;
    }

    // Keep preview navigation in topic/level order even when cards are grouped by status.
    QSqlQuery stagesQuery(database);
    if (!stagesQuery.prepare(QStringLiteral(
                "WITH passed AS ("
                " SELECT DISTINCT s.stage_id FROM stages s JOIN stage_records r ON r.stage_id = s.stage_id"
                " WHERE r.user_id = ? AND r.score >= s.pass_score"
                "), passed_levels AS ("
                " SELECT DISTINCT s.cefr_level FROM stages s JOIN passed ON passed.stage_id = s.stage_id"
                "), stage_status AS ("
                " SELECT s.stage_id, s.topic_id, s.cefr_level, s.time_limit, s.question_count, s.pass_score,"
                "        passed.stage_id IS NOT NULL AS is_solved,"
                "        (s.cefr_level = 'A1' OR previous_pass.cefr_level IS NOT NULL) AS is_unlocked,"
                "        LEAD(s.stage_id) OVER (ORDER BY s.topic_id,"
                "          CASE s.cefr_level WHEN 'A1' THEN 1 WHEN 'A2' THEN 2 WHEN 'B1' THEN 3"
                "          WHEN 'B2' THEN 4 WHEN 'C1' THEN 5 WHEN 'C2' THEN 6 END, s.stage_id) AS next_stage_id"
                " FROM stages s LEFT JOIN passed ON passed.stage_id = s.stage_id"
                " LEFT JOIN passed_levels previous_pass ON previous_pass.cefr_level ="
                "   CASE s.cefr_level WHEN 'A2' THEN 'A1' WHEN 'B1' THEN 'A2' WHEN 'B2' THEN 'B1'"
                "   WHEN 'C1' THEN 'B2' WHEN 'C2' THEN 'C1' END"
                ") SELECT stage_id, topic_id, cefr_level, time_limit, question_count, pass_score,"
                " is_unlocked, next_stage_id FROM stage_status"
                " ORDER BY CASE WHEN is_solved THEN 0 WHEN is_unlocked THEN 1 ELSE 2 END,"
                " CASE cefr_level WHEN 'A1' THEN 1 WHEN 'A2' THEN 2 WHEN 'B1' THEN 3"
                " WHEN 'B2' THEN 4 WHEN 'C1' THEN 5 WHEN 'C2' THEN 6 END, stage_id"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStagesWithUnlockStatus.stages.prepare",
                                                stagesQuery);
        return std::nullopt;
    }
    stagesQuery.addBindValue(userId);
    if (!stagesQuery.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStagesWithUnlockStatus.stages", stagesQuery);
        return std::nullopt;
    }

    while (stagesQuery.next()) {
        StageStatusDTO status;
        status.stage = SqliteRepositoryUtils::mapStage(stagesQuery);
        status.is_unlocked = stagesQuery.value(6).toBool();
        status.preview_next_stage_id = stagesQuery.value(7).toInt();
        results.push_back(std::move(status));
    }
    return results;
}

std::optional<std::vector<StageMapItem>> StageRepository::getStageMapForUser(int userId) {
    if (userId <= 0) {
        qWarning() << "StageRepository::getStageMapForUser received an invalid user id.";
        return std::nullopt;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "StageRepository", "getStageMapForUser")) {
        return std::nullopt;
    }

    QSqlQuery userQuery(database);
    if (!userQuery.prepare(QStringLiteral("SELECT 1 FROM users WHERE user_id = ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStageMapForUser.user.prepare", userQuery);
        return std::nullopt;
    }
    userQuery.addBindValue(userId);
    if (!userQuery.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStageMapForUser.user", userQuery);
        return std::nullopt;
    }
    if (!userQuery.next()) {
        qWarning() << "StageRepository::getStageMapForUser found no user with id" << userId;
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
                "WITH ranked_records AS ("
                " SELECT r.stage_id, r.score, r.completion_time,"
                "        ROW_NUMBER() OVER (PARTITION BY r.stage_id"
                "          ORDER BY r.score DESC, r.completion_time ASC, r.played_at ASC, r.record_id ASC) AS attempt_rank"
                " FROM stage_records r WHERE r.user_id = ?"
                "), best_records AS ("
                " SELECT stage_id, score AS best_score, completion_time AS best_time"
                " FROM ranked_records WHERE attempt_rank = 1"
                "), passed_levels AS ("
                " SELECT DISTINCT s.cefr_level"
                " FROM stages s JOIN best_records b ON b.stage_id = s.stage_id"
                " WHERE b.best_score >= s.pass_score"
                ")"
                " SELECT s.stage_id, s.topic_id, t.topic_name, s.cefr_level, s.question_count, s.time_limit,"
                "        s.pass_score,"
                "        CASE WHEN s.cefr_level = 'A1' OR previous_pass.cefr_level IS NOT NULL THEN 1 ELSE 0 END,"
                "        CASE WHEN b.best_score IS NOT NULL AND b.best_score >= s.pass_score THEN 1 ELSE 0 END,"
                "        b.best_score, b.best_time"
                " FROM topics t JOIN stages s ON s.topic_id = t.topic_id"
                " LEFT JOIN best_records b ON b.stage_id = s.stage_id"
                " LEFT JOIN passed_levels previous_pass ON previous_pass.cefr_level ="
                "   CASE s.cefr_level WHEN 'A2' THEN 'A1' WHEN 'B1' THEN 'A2' WHEN 'B2' THEN 'B1'"
                "   WHEN 'C1' THEN 'B2' WHEN 'C2' THEN 'C1' END"
                " ORDER BY t.topic_id,"
                "   CASE s.cefr_level WHEN 'A1' THEN 1 WHEN 'A2' THEN 2 WHEN 'B1' THEN 3"
                "   WHEN 'B2' THEN 4 WHEN 'C1' THEN 5 WHEN 'C2' THEN 6 END, s.stage_id"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStageMapForUser.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(userId);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getStageMapForUser", query);
        return std::nullopt;
    }

    std::vector<StageMapItem> items;
    while (query.next()) {
        StageMapItem item;
        item.stage_id = query.value(0).toInt();
        item.topic_id = query.value(1).toInt();
        item.topic_name = query.value(2).toString().toStdString();
        item.cefr_level = SqliteRepositoryUtils::stringColumn(query, 3);
        item.question_count = query.value(4).toInt();
        item.time_limit = query.value(5).toInt();
        item.pass_score = query.value(6).toInt();
        item.is_unlocked = query.value(7).toBool();
        item.is_passed = query.value(8).toBool();
        item.best_score = query.value(9).toInt();
        item.best_time = query.value(10).toInt();
        items.push_back(std::move(item));
    }
    return items;
}

std::vector<WordItem> StageRepository::getRandomWords(int topicId, CEFRLevel level, int limit) {
    std::vector<WordItem> words;
    if (topicId <= 0 || limit <= 0) {
        qWarning() << "StageRepository::getRandomWords received invalid topic or limit values.";
        return words;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!SqliteRepositoryUtils::databaseAvailable(database, "StageRepository", "getRandomWords")) {
        return words;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral("SELECT word_id, topic_id, cefr_level, target_word, hint_image_path, audio_path "
                                      "FROM word_pool WHERE topic_id = ? AND cefr_level = ? "
                                      "AND TRIM(target_word) <> '' "
                                      "ORDER BY RANDOM() LIMIT ?"))) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getRandomWords.prepare", query);
        return words;
    }
    query.addBindValue(topicId);
    query.addBindValue(QString::fromStdString(cefrLevelName(level)));
    query.addBindValue(limit);
    if (!query.exec()) {
        SqliteRepositoryUtils::warnQueryFailure("StageRepository", "getRandomWords", query);
        return words;
    }
    while (query.next()) {
        words.push_back(SqliteRepositoryUtils::mapWordItem(query));
    }
    return words;
}
