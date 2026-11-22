#include "SqliteVocabularyRepository.h"

#include "../database/DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include <utility>

namespace {

std::string stringColumn(const QSqlQuery& query, int column) {
    const QVariant value = query.value(column);
    if (value.isNull()) {
        return {};
    }
    return value.toString().toStdString();
}

Vocabulary mapVocabulary(const QSqlQuery& query) {
    Vocabulary vocabulary;
    vocabulary.id = query.value(0).toInt();
    vocabulary.topicId = query.value(1).toInt();
    vocabulary.word = stringColumn(query, 2);
    vocabulary.phonetic = stringColumn(query, 3);
    vocabulary.partOfSpeech = stringColumn(query, 4);
    vocabulary.cefrLevel = stringColumn(query, 5);
    vocabulary.meaning = stringColumn(query, 6);
    vocabulary.exampleSentence = stringColumn(query, 7);
    return vocabulary;
}

}  // namespace

SqliteVocabularyRepository::SqliteVocabularyRepository(DatabaseManager& databaseManager)
    : m_databaseManager(databaseManager) {}

std::vector<Topic> SqliteVocabularyRepository::getAllTopics() {
    std::vector<Topic> topics;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        return topics;
    }

    QSqlQuery query(database);
    if (!query.exec(QStringLiteral("SELECT id, name, description FROM topics ORDER BY id"))) {
        return topics;
    }

    while (query.next()) {
        Topic topic;
        topic.id = query.value(0).toInt();
        topic.name = stringColumn(query, 1);
        topic.description = stringColumn(query, 2);
        topics.push_back(std::move(topic));
    }
    return topics;
}

std::vector<Vocabulary> SqliteVocabularyRepository::getVocabulariesByTopic(int topicId) {
    std::vector<Vocabulary> vocabularies;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        return vocabularies;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT id, topic_id, word, phonetic, part_of_speech, cefr_level, meaning, example_sentence "
            "FROM vocabularies WHERE topic_id = ? ORDER BY id"))) {
        return vocabularies;
    }
    query.addBindValue(topicId);
    if (!query.exec()) {
        return vocabularies;
    }

    while (query.next()) {
        vocabularies.push_back(mapVocabulary(query));
    }
    return vocabularies;
}

std::vector<Vocabulary> SqliteVocabularyRepository::getVocabulariesByLevel(const std::string& cefrLevel) {
    std::vector<Vocabulary> vocabularies;
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        return vocabularies;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT id, topic_id, word, phonetic, part_of_speech, cefr_level, meaning, example_sentence "
            "FROM vocabularies WHERE cefr_level = ? ORDER BY id"))) {
        return vocabularies;
    }
    query.addBindValue(QString::fromStdString(cefrLevel));
    if (!query.exec()) {
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
        return vocabularies;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT id, topic_id, word, phonetic, part_of_speech, cefr_level, meaning, example_sentence "
            "FROM vocabularies "
            "WHERE (? = -1 OR topic_id = ?) "
            "AND (? = '' OR cefr_level = ?) "
            "ORDER BY RANDOM() LIMIT ?"))) {
        return vocabularies;
    }
    query.addBindValue(topicId);
    query.addBindValue(topicId);
    const QString level = QString::fromStdString(cefrLevel);
    query.addBindValue(level);
    query.addBindValue(level);
    query.addBindValue(limit);
    if (!query.exec()) {
        return vocabularies;
    }

    while (query.next()) {
        vocabularies.push_back(mapVocabulary(query));
    }
    return vocabularies;
}

std::optional<LearningProgress> SqliteVocabularyRepository::getProgressByVocabId(int vocabId) {
    const QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen()) {
        return std::nullopt;
    }

    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT vocab_id, correct_count, wrong_count, mastery_level, last_reviewed "
            "FROM learning_progress WHERE vocab_id = ?"))) {
        return std::nullopt;
    }
    query.addBindValue(vocabId);
    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    LearningProgress progress;
    progress.vocabId = query.value(0).toInt();
    progress.correctCount = query.value(1).toInt();
    progress.wrongCount = query.value(2).toInt();
    progress.masteryLevel = query.value(3).toInt();
    progress.lastReviewed = stringColumn(query, 4);
    return progress;
}

bool SqliteVocabularyRepository::updateLearningProgress(int vocabId, bool isCorrect) {
    QSqlDatabase database = m_databaseManager.connection();
    if (!database.isOpen() || !database.transaction()) {
        return false;
    }

    bool exists = false;
    {
        QSqlQuery query(database);
        if (!query.prepare(QStringLiteral(
                "SELECT 1 FROM learning_progress WHERE vocab_id = ? LIMIT 1"))) {
            database.rollback();
            return false;
        }
        query.addBindValue(vocabId);
        if (!query.exec()) {
            database.rollback();
            return false;
        }
        exists = query.next();
    }

    const int correctIncrement = isCorrect ? 1 : 0;
    const int wrongIncrement = isCorrect ? 0 : 1;
    bool succeeded = false;

    if (exists) {
        QSqlQuery query(database);
        succeeded = query.prepare(QStringLiteral(
            "UPDATE learning_progress "
            "SET correct_count = correct_count + ?, "
            "wrong_count = wrong_count + ?, "
            "mastery_level = CASE "
            "WHEN correct_count + ? >= 3 THEN 2 "
            "WHEN correct_count + ? >= 1 THEN 1 "
            "ELSE 0 END, "
            "last_reviewed = datetime('now') "
            "WHERE vocab_id = ?"));
        if (succeeded) {
            query.addBindValue(correctIncrement);
            query.addBindValue(wrongIncrement);
            query.addBindValue(correctIncrement);
            query.addBindValue(correctIncrement);
            query.addBindValue(vocabId);
            succeeded = query.exec() && query.numRowsAffected() == 1;
        }
    } else {
        const int initialMastery = correctIncrement == 1 ? 1 : 0;
        QSqlQuery query(database);
        succeeded = query.prepare(QStringLiteral(
            "INSERT INTO learning_progress "
            "(vocab_id, correct_count, wrong_count, mastery_level, last_reviewed) "
            "VALUES (?, ?, ?, ?, datetime('now'))"));
        if (succeeded) {
            query.addBindValue(vocabId);
            query.addBindValue(correctIncrement);
            query.addBindValue(wrongIncrement);
            query.addBindValue(initialMastery);
            succeeded = query.exec() && query.numRowsAffected() == 1;
        }
    }

    if (!succeeded) {
        database.rollback();
        return false;
    }
    if (!database.commit()) {
        database.rollback();
        return false;
    }
    return true;
}
