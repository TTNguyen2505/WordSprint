// VocabularyRepository.cpp
// Concrete implementation of IVocabularyRepository using QtSql.

#include "VocabularyRepository.h"
#include "Level.h"
#include "Topic.h"
#include "Vocabulary.h"

#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDateTime>
#include <optional>
#include <sstream>

VocabularyRepository::VocabularyRepository(std::shared_ptr<DatabaseManager> dbMgr)
    : m_dbMgr(std::move(dbMgr)) {}

// ---------------------------------------------------------------------------
// Helper mappers – convert the current row of a QSqlQuery into a struct.
// ---------------------------------------------------------------------------
Level VocabularyRepository::mapToLevel(const QSqlQuery &q) {
    Level lvl;
    lvl.id = q.value("id").toInt();
    lvl.code = q.value("code").toString().toStdString();
    lvl.name = q.value("name").toString().toStdString();
    lvl.description = q.value("description").toString().toStdString();
    return lvl;
}

Topic VocabularyRepository::mapToTopic(const QSqlQuery &q) {
    Topic tp;
    tp.id = q.value("id").toInt();
    tp.name = q.value("name").toString().toStdString();
    tp.description = q.value("description").toString().toStdString();
    return tp;
}

Vocabulary VocabularyRepository::mapToVocabulary(const QSqlQuery &q) {
    Vocabulary voc;
    voc.id = q.value("id").toInt();
    voc.word = q.value("word").toString().toStdString();
    voc.phonetic = q.value("phonetic").toString().toStdString();
    voc.part_of_speech = q.value("part_of_speech").toString().toStdString();
    voc.meaning_vi = q.value("meaning_vi").toString().toStdString();
    voc.definition_en = q.value("definition_en").toString().toStdString();
    voc.example_sentence = q.value("example_sentence").toString().toStdString();
    voc.example_translation = q.value("example_translation").toString().toStdString();
    voc.level_id = q.value("level_id").toInt();
    voc.topic_id = q.value("topic_id").toInt();
    // Assuming `created_at` is stored as INTEGER Unix epoch (seconds).
    qint64 ts = q.value("created_at").toLongLong();
    if (ts != 0) {
        voc.created_at = std::chrono::system_clock::time_point(std::chrono::seconds(ts));
    }
    return voc;
}

// ---------------------------------------------------------------------------
// Public repository methods.
// ---------------------------------------------------------------------------
std::vector<Level> VocabularyRepository::getAllLevels() {
    std::vector<Level> result;
    QString sql = "SELECT id, code, name, description FROM levels ORDER BY id;";
    QSqlQuery query = m_dbMgr->prepare(sql);
    if (!query.exec()) {
        qWarning() << "getAllLevels failed:" << query.lastError().text();
        return result;
    }
    while (query.next()) {
        result.push_back(mapToLevel(query));
    }
    return result;
}

std::vector<Topic> VocabularyRepository::getAllTopics() {
    std::vector<Topic> result;
    QString sql = "SELECT id, name, description FROM topics ORDER BY id;";
    QSqlQuery query = m_dbMgr->prepare(sql);
    if (!query.exec()) {
        qWarning() << "getAllTopics failed:" << query.lastError().text();
        return result;
    }
    while (query.next()) {
        result.push_back(mapToTopic(query));
    }
    return result;
}

std::vector<Vocabulary> VocabularyRepository::getVocabulariesByLevel(int levelId) {
    std::vector<Vocabulary> result;
    QString sql = "SELECT * FROM vocabularies WHERE level_id = :lvl;";
    QSqlQuery query = m_dbMgr->prepare(sql);
    query.bindValue(":lvl", levelId);
    if (!query.exec()) {
        qWarning() << "getVocabulariesByLevel failed:" << query.lastError().text();
        return result;
    }
    while (query.next()) {
        result.push_back(mapToVocabulary(query));
    }
    return result;
}

std::vector<Vocabulary> VocabularyRepository::getVocabulariesByTopic(int topicId) {
    std::vector<Vocabulary> result;
    QString sql = "SELECT * FROM vocabularies WHERE topic_id = :tp;";
    QSqlQuery query = m_dbMgr->prepare(sql);
    query.bindValue(":tp", topicId);
    if (!query.exec()) {
        qWarning() << "getVocabulariesByTopic failed:" << query.lastError().text();
        return result;
    }
    while (query.next()) {
        result.push_back(mapToVocabulary(query));
    }
    return result;
}

std::vector<Vocabulary> VocabularyRepository::getVocabulariesFilter(const std::string &filter,
                                                                   const std::vector<std::string> &params) {
    std::vector<Vocabulary> result;
    // Build the final query string safely – filter is supplied by the caller (trusted internal code).
    QString sql = QString::fromStdString("SELECT * FROM vocabularies WHERE " + filter + ";");
    QSqlQuery query = m_dbMgr->prepare(sql);
    // Bind parameters in order.
    for (size_t i = 0; i < params.size(); ++i) {
        query.bindValue(static_cast<int>(i), QString::fromStdString(params[i]));
    }
    if (!query.exec()) {
        qWarning() << "getVocabulariesFilter exec error:" << query.lastError().text();
        return result;
    }
    while (query.next()) {
        result.push_back(mapToVocabulary(query));
    }
    return result;
}

std::optional<Vocabulary> VocabularyRepository::getVocabularyById(int vocabId) {
    QString sql = "SELECT * FROM vocabularies WHERE id = :id;";
    QSqlQuery query = m_dbMgr->prepare(sql);
    query.bindValue(":id", vocabId);
    if (!query.exec()) {
        qWarning() << "getVocabularyById exec error:" << query.lastError().text();
        return std::nullopt;
    }
    if (query.next()) {
        return mapToVocabulary(query);
    }
    return std::nullopt;
}

std::vector<Vocabulary> VocabularyRepository::searchVocabularies(const std::string &keyword) {
    std::vector<Vocabulary> result;
    // Simple case‑insensitive LIKE search across a few textual columns.
    QString pattern = QString::fromStdString("%" + keyword + "%");
    QString sql = R"(
        SELECT * FROM vocabularies
        WHERE word LIKE :kw COLLATE NOCASE
           OR meaning_vi LIKE :kw COLLATE NOCASE
           OR definition_en LIKE :kw COLLATE NOCASE
           OR example_sentence LIKE :kw COLLATE NOCASE
    )";
    QSqlQuery query = m_dbMgr->prepare(sql);
    query.bindValue(":kw", pattern);
    if (!query.exec()) {
        qWarning() << "searchVocabularies exec error:" << query.lastError().text();
        return result;
    }
    while (query.next()) {
        result.push_back(mapToVocabulary(query));
    }
    return result;
}
