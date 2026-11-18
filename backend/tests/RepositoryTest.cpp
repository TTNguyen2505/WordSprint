// RepositoryTest.cpp
// Simple in‑memory test for VocabularyRepository.

#include <QtTest/QtTest>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "DatabaseManager.h"
#include "VocabularyRepository.h"
#include "Level.h"
#include "Topic.h"
#include "Vocabulary.h"

class RepositoryTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();   // runs once before all tests
    void cleanupTestCase();
    void testLevels();
    void testTopics();
    void testVocabularyByLevel();
    void testSearch();

private:
    std::shared_ptr<DatabaseManager> m_dbMgr;
    std::unique_ptr<VocabularyRepository> m_repo;
};

void RepositoryTest::initTestCase() {
    // In‑memory SQLite DB
    m_dbMgr = std::make_shared<DatabaseManager>(QStringLiteral(":memory:"));
    // Build schema
    QSqlQuery q(m_dbMgr->db());
    // levels
    QVERIFY(q.exec("CREATE TABLE levels (id INTEGER PRIMARY KEY, code TEXT, name TEXT, description TEXT);"));
    // topics
    QVERIFY(q.exec("CREATE TABLE topics (id INTEGER PRIMARY KEY, name TEXT, description TEXT);"));
    // vocabularies – created_at stored as INTEGER epoch seconds
    QVERIFY(q.exec("CREATE TABLE vocabularies (id INTEGER PRIMARY KEY, word TEXT, phonetic TEXT, part_of_speech TEXT, meaning_vi TEXT, definition_en TEXT, example_sentence TEXT, example_translation TEXT, level_id INTEGER, topic_id INTEGER, created_at INTEGER, FOREIGN KEY(level_id) REFERENCES levels(id), FOREIGN KEY(topic_id) REFERENCES topics(id));"));
    // Insert seed data
    QVERIFY(q.exec("INSERT INTO levels (id, code, name, description) VALUES (1, 'A1', 'Beginner', 'Basic level');"));
    QVERIFY(q.exec("INSERT INTO topics (id, name, description) VALUES (1, 'General', 'General vocab');"));
    QVERIFY(q.exec("INSERT INTO vocabularies (id, word, phonetic, part_of_speech, meaning_vi, definition_en, example_sentence, example_translation, level_id, topic_id, created_at) VALUES (1, 'apple', 'ˈæpəl', 'noun', 'quả táo', 'A fruit', 'I eat an apple.', 'Tôi ăn một quả táo.', 1, 1, strftime('%s','now'));"));
    // Repository instance
    m_repo = std::make_unique<VocabularyRepository>(m_dbMgr);
}

void RepositoryTest::cleanupTestCase() {
    // Database will be closed automatically when m_dbMgr is destroyed.
    m_repo.reset();
    m_dbMgr.reset();
}

void RepositoryTest::testLevels() {
    auto levels = m_repo->getAllLevels();
    QCOMPARE(levels.size(), size_t(1));
    QCOMPARE(levels[0].code, std::string("A1"));
}

void RepositoryTest::testTopics() {
    auto topics = m_repo->getAllTopics();
    QCOMPARE(topics.size(), size_t(1));
    QCOMPARE(topics[0].name, std::string("General"));
}

void RepositoryTest::testVocabularyByLevel() {
    auto vocab = m_repo->getVocabulariesByLevel(1);
    QCOMPARE(vocab.size(), size_t(1));
    QCOMPARE(vocab[0].word, std::string("apple"));
    // Verify created_at is set (non‑zero epoch)
    QVERIFY(vocab[0].created_at.time_since_epoch().count() > 0);
}

void RepositoryTest::testSearch() {
    auto result = m_repo->searchVocabularies("apple");
    QCOMPARE(result.size(), size_t(1));
    QCOMPARE(result[0].meaning_vi, std::string("quả táo"));
}

QTEST_MAIN(RepositoryTest)
#include "RepositoryTest.moc"
