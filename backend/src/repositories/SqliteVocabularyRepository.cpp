#include "SqliteVocabularyRepository.h"

#include "../database/DatabaseManager.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {

void warnQueryFailure(const char* operation, const QSqlQuery& query) {
    qWarning().noquote() << "SqliteVocabularyRepository::" << operation
                         << "failed:" << query.lastError().text();
}

void warnDatabaseUnavailable(const char* operation) {
    qWarning() << "SqliteVocabularyRepository::" << operation
               << "could not run because the database connection is not open.";
}

std::string stringColumn(const QSqlQuery& query, int column) {
    const QVariant value = query.value(column);
    return value.isNull() ? std::string{} : value.toString().toStdString();
}

std::optional<std::string> optionalStringColumn(const QSqlQuery& query, int column) {
    const QVariant value = query.value(column);
    if (value.isNull()) {
        return std::nullopt;
    }
    return value.toString().toStdString();
}

std::optional<int> optionalIntColumn(const QSqlQuery& query, int column) {
    const QVariant value = query.value(column);
    if (value.isNull()) {
        return std::nullopt;
    }

    bool ok = false;
    const int result = value.toInt(&ok);
    return ok ? std::optional<int>{result} : std::nullopt;
}

Topic mapTopic(const QSqlQuery& query) {
    Topic topic;
    topic.topic_id = query.value(0).toInt();
    topic.topic_name = stringColumn(query, 1);
    topic.description = optionalStringColumn(query, 2);
    return topic;
}

Stage mapStage(const QSqlQuery& query) {
    Stage stage;
    stage.stage_id = query.value(0).toInt();
    stage.topic_id = query.value(1).toInt();
    stage.cefr_level = stringColumn(query, 2);
    stage.unlock_order = query.value(3).toInt();
    stage.time_limit = query.value(4).toInt();
    stage.question_count = query.value(5).toInt();
    return stage;
}

Vocabulary mapVocabulary(const QSqlQuery& query) {
    Vocabulary vocabulary;
    vocabulary.word_id = query.value(0).toInt();
    vocabulary.topic_id = query.value(1).toInt();
    vocabulary.cefr_level = stringColumn(query, 2);
    vocabulary.target_word = stringColumn(query, 3);
    vocabulary.hint_text = optionalStringColumn(query, 4);
    vocabulary.image_path = optionalStringColumn(query, 5);
    vocabulary.audio_path = stringColumn(query, 6);
    return vocabulary;
}

StageRecord mapStageRecord(const QSqlQuery& query) {
    StageRecord record;
    record.record_id = query.value(0).toInt();
    record.user_id = query.value(1).toInt();
    record.stage_id = query.value(2).toInt();
    record.score = query.value(3).toInt();
    record.completion_time = query.value(4).toInt();
    record.played_at = stringColumn(query, 5);
    return record;
}

UserProgress mapUserProgress(const QSqlQuery& query) {
    UserProgress progress;
    progress.user_id = query.value(0).toInt();
    progress.stage_id = query.value(1).toInt();
    progress.is_unlocked = query.value(2).toInt();
    progress.is_completed = query.value(3).toInt();
    progress.high_score = query.value(4).toInt();
    progress.best_time = optionalIntColumn(query, 5);
    return progress;
}

}  // namespace

SqliteVocabularyRepository::SqliteVocabularyRepository(DatabaseManager& databaseManager)
    : m_databaseManager(databaseManager) {}

std::vector<Topic> SqliteVocabularyRepository::getAllTopics() {
    std::vector<Topic> topics;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getAllTopics");
        return topics;
    }

    QSqlQuery query(database);
    if (!query.exec(QStringLiteral(
            "SELECT topic_id, topic_name, description "
            "FROM topics ORDER BY topic_id"))) {
        warnQueryFailure("getAllTopics", query);
        return topics;
    }

    while (query.next()) {
        topics.push_back(mapTopic(query));
    }
    return topics;
}

std::vector<Stage> SqliteVocabularyRepository::getStagesByTopicId(int topic_id) {
    std::vector<Stage> stages;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getStagesByTopicId");
        return stages;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT stage_id, topic_id, cefr_level, unlock_order, time_limit, question_count "
            "FROM stages WHERE topic_id = ? ORDER BY unlock_order"))) {
        warnQueryFailure("getStagesByTopicId.prepare", query);
        return stages;
    }
    query.addBindValue(topic_id);
    if (!query.exec()) {
        warnQueryFailure("getStagesByTopicId", query);
        return stages;
    }

    while (query.next()) {
        stages.push_back(mapStage(query));
    }
    return stages;
}

std::optional<Stage> SqliteVocabularyRepository::getStageById(int stage_id) {
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getStageById");
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT stage_id, topic_id, cefr_level, unlock_order, time_limit, question_count "
            "FROM stages WHERE stage_id = ?"))) {
        warnQueryFailure("getStageById.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(stage_id);
    if (!query.exec()) {
        warnQueryFailure("getStageById", query);
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }
    return mapStage(query);
}

std::vector<Vocabulary> SqliteVocabularyRepository::getVocabulariesByTopic(int topicId) {
    std::vector<Vocabulary> vocabularies;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getVocabulariesByTopic");
        return vocabularies;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT word_id, topic_id, cefr_level, target_word, hint_text, image_path, audio_path "
            "FROM word_pool WHERE topic_id = ? ORDER BY word_id"))) {
        warnQueryFailure("getVocabulariesByTopic.prepare", query);
        return vocabularies;
    }
    query.addBindValue(topicId);
    if (!query.exec()) {
        warnQueryFailure("getVocabulariesByTopic", query);
        return vocabularies;
    }

    while (query.next()) {
        vocabularies.push_back(mapVocabulary(query));
    }
    return vocabularies;
}

std::vector<Vocabulary> SqliteVocabularyRepository::getVocabulariesByLevel(
    const std::string& cefrLevel) {
    std::vector<Vocabulary> vocabularies;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getVocabulariesByLevel");
        return vocabularies;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT word_id, topic_id, cefr_level, target_word, hint_text, image_path, audio_path "
            "FROM word_pool WHERE cefr_level = ? ORDER BY word_id"))) {
        warnQueryFailure("getVocabulariesByLevel.prepare", query);
        return vocabularies;
    }
    query.addBindValue(QString::fromStdString(cefrLevel));
    if (!query.exec()) {
        warnQueryFailure("getVocabulariesByLevel", query);
        return vocabularies;
    }

    while (query.next()) {
        vocabularies.push_back(mapVocabulary(query));
    }
    return vocabularies;
}

std::vector<Vocabulary> SqliteVocabularyRepository::getRandomVocabularies(
    int limit,
    int topicId,
    const std::string& cefrLevel) {
    std::vector<Vocabulary> vocabularies;
    if (limit <= 0) {
        return vocabularies;
    }

    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getRandomVocabularies");
        return vocabularies;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT word_id, topic_id, cefr_level, target_word, hint_text, image_path, audio_path "
            "FROM word_pool "
            "WHERE (? = -1 OR topic_id = ?) "
            "AND (? = '' OR cefr_level = ?) "
            "ORDER BY RANDOM() LIMIT ?"))) {
        warnQueryFailure("getRandomVocabularies.prepare", query);
        return vocabularies;
    }
    query.addBindValue(topicId);
    query.addBindValue(topicId);
    const QString level = QString::fromStdString(cefrLevel);
    query.addBindValue(level);
    query.addBindValue(level);
    query.addBindValue(limit);
    if (!query.exec()) {
        warnQueryFailure("getRandomVocabularies", query);
        return vocabularies;
    }

    while (query.next()) {
        vocabularies.push_back(mapVocabulary(query));
    }
    return vocabularies;
}

std::vector<StageRecord> SqliteVocabularyRepository::getStageRecordsByUserId(int user_id) {
    std::vector<StageRecord> records;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getStageRecordsByUserId");
        return records;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT record_id, user_id, stage_id, score, completion_time, played_at "
            "FROM stage_records WHERE user_id = ? ORDER BY played_at DESC, record_id DESC"))) {
        warnQueryFailure("getStageRecordsByUserId.prepare", query);
        return records;
    }
    query.addBindValue(user_id);
    if (!query.exec()) {
        warnQueryFailure("getStageRecordsByUserId", query);
        return records;
    }

    while (query.next()) {
        records.push_back(mapStageRecord(query));
    }
    return records;
}

std::optional<int> SqliteVocabularyRepository::insertStageRecord(
    int user_id,
    int stage_id,
    int score,
    int completion_time) {
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("insertStageRecord");
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO stage_records (user_id, stage_id, score, completion_time) "
            "VALUES (?, ?, ?, ?)"))) {
        warnQueryFailure("insertStageRecord.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(user_id);
    query.addBindValue(stage_id);
    query.addBindValue(score);
    query.addBindValue(completion_time);
    if (!query.exec()) {
        warnQueryFailure("insertStageRecord", query);
        return std::nullopt;
    }

    bool ok = false;
    const int record_id = query.lastInsertId().toInt(&ok);
    if (!ok) {
        qWarning() << "SqliteVocabularyRepository::insertStageRecord succeeded, but SQLite did not return record_id.";
        return std::nullopt;
    }
    return record_id;
}

std::optional<UserProgress> SqliteVocabularyRepository::getUserProgress(int user_id, int stage_id) {
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("getUserProgress");
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT user_id, stage_id, is_unlocked, is_completed, high_score, best_time "
            "FROM user_progress WHERE user_id = ? AND stage_id = ?"))) {
        warnQueryFailure("getUserProgress.prepare", query);
        return std::nullopt;
    }
    query.addBindValue(user_id);
    query.addBindValue(stage_id);
    if (!query.exec()) {
        warnQueryFailure("getUserProgress", query);
        return std::nullopt;
    }
    if (!query.next()) {
        return std::nullopt;
    }
    return mapUserProgress(query);
}

bool SqliteVocabularyRepository::saveUserProgress(const UserProgress& progress) {
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("saveUserProgress");
        return false;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO user_progress "
            "(user_id, stage_id, is_unlocked, is_completed, high_score, best_time) "
            "VALUES (?, ?, ?, ?, ?, ?) "
            "ON CONFLICT(user_id, stage_id) DO UPDATE SET "
            "is_unlocked = excluded.is_unlocked, "
            "is_completed = excluded.is_completed, "
            "high_score = excluded.high_score, "
            "best_time = excluded.best_time"))) {
        warnQueryFailure("saveUserProgress.prepare", query);
        return false;
    }

    QVariant best_time;
    if (progress.best_time.has_value()) {
        best_time = *progress.best_time;
    }
    query.addBindValue(progress.user_id);
    query.addBindValue(progress.stage_id);
    query.addBindValue(progress.is_unlocked);
    query.addBindValue(progress.is_completed);
    query.addBindValue(progress.high_score);
    query.addBindValue(best_time);
    if (!query.exec()) {
        warnQueryFailure("saveUserProgress", query);
        return false;
    }
    return true;
}

bool SqliteVocabularyRepository::unlockStage(int user_id, int stage_id) {
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        warnDatabaseUnavailable("unlockStage");
        return false;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO user_progress "
            "(user_id, stage_id, is_unlocked, is_completed, high_score, best_time) "
            "VALUES (?, ?, 1, 0, 0, NULL) "
            "ON CONFLICT(user_id, stage_id) DO UPDATE SET is_unlocked = 1"))) {
        warnQueryFailure("unlockStage.prepare", query);
        return false;
    }
    query.addBindValue(user_id);
    query.addBindValue(stage_id);
    if (!query.exec()) {
        warnQueryFailure("unlockStage", query);
        return false;
    }
    return true;
}
