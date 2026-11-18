// ViewModelIntegrationTest.cpp
// Integration test: ensure VocabularyViewModel loads data from SQLite via Repository.

#include <QtTest/QtTest>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "VocabularyViewModel.h"
#include "DatabaseManager.h"
#include "VocabularyRepository.h"

class ViewModelIntegrationTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();   // set up in‑memory DB
    void cleanupTestCase();
    void testLoadLevels();
    void testLoadTopics();
    void testLoadVocabularies();
    void testFilterByLevel();
    void testSearch();

private:
    std::shared_ptr<DatabaseManager> m_dbMgr;
    std::unique_ptr<VocabularyViewModel> m_vm;
};

void ViewModelIntegrationTest::initTestCase() {
    // In‑memory SQLite DB
    m_dbMgr = std::make_shared<DatabaseManager>(QStringLiteral(":memory:"));
    // Create schema (same as RepositoryTest)
    QSqlQuery q(m_dbMgr->db());
    QVERIFY(q.exec("CREATE TABLE levels (id INTEGER PRIMARY KEY, code TEXT, name TEXT, description TEXT);"));
    QVERIFY(q.exec("CREATE TABLE topics (id INTEGER PRIMARY KEY, name TEXT, description TEXT);"));
    QVERIFY(q.exec("CREATE TABLE vocabularies (id INTEGER PRIMARY KEY, word TEXT, phonetic TEXT, part_of_speech TEXT, meaning_vi TEXT, definition_en TEXT, example_sentence TEXT, example_translation TEXT, level_id INTEGER, topic_id INTEGER, created_at INTEGER, FOREIGN KEY(level_id) REFERENCES levels(id), FOREIGN KEY(topic_id) REFERENCES topics(id));"));
    // seed data
    QVERIFY(q.exec("INSERT INTO levels (id, code, name, description) VALUES (1, 'A1', 'Beginner', 'Basic'), (2, 'B1', 'Intermediate', 'Mid');"));
    QVERIFY(q.exec("INSERT INTO topics (id, name, description) VALUES (1, 'General', 'General vocab'), (2, 'Tech', 'Technology');"));
    QVERIFY(q.exec("INSERT INTO vocabularies (id, word, phonetic, part_of_speech, meaning_vi, definition_en, example_sentence, example_translation, level_id, topic_id, created_at) VALUES (1, 'apple', 'ˈæpəl', 'noun', 'quả táo', 'A fruit', 'I eat an apple.', 'Tôi ăn một quả táo.', 1, 1, strftime('%s','now'));"));
    QVERIFY(q.exec("INSERT INTO vocabularies (id, word, phonetic, part_of_speech, meaning_vi, definition_en, example_sentence, example_translation, level_id, topic_id, created_at) VALUES (2, 'computer', 'kəmˈpjuːtə', 'noun', 'máy tính', 'A device', 'I use a computer.', 'Tôi dùng máy tính.', 2, 2, strftime('%s','now'));"));

    // Create ViewModel – it will load data in constructor
    m_vm = std::make_unique<VocabularyViewModel>(m_dbMgr);
}

void ViewModelIntegrationTest::cleanupTestCase() {
    m_vm.reset();
    m_dbMgr.reset();
}

void ViewModelIntegrationTest::testLoadLevels() {
    QCOMPARE(m_vm->levels().size(), 2);
    QVariantMap lvl0 = m_vm->levels().at(0).toMap();
    QVERIFY(lvl0["code"].toString() == "A1" || lvl0["code"].toString() == "B1");
}

void ViewModelIntegrationTest::testLoadTopics() {
    QCOMPARE(m_vm->topics().size(), 2);
    QVariantMap top0 = m_vm->topics().at(0).toMap();
    QVERIFY(top0["name"].toString() == "General" || top0["name"].toString() == "Tech");
}

void ViewModelIntegrationTest::testLoadVocabularies() {
    // Constructor already called loadVocabularies()
    QCOMPARE(m_vm->vocabularies().size(), 2);
}

void ViewModelIntegrationTest::testFilterByLevel() {
    m_vm->filterByLevel(1);
    QCOMPARE(m_vm->vocabularies().size(), 1);
    QVariantMap voc = m_vm->vocabularies().at(0).toMap();
    QCOMPARE(voc["word"].toString(), QStringLiteral("apple"));
}

void ViewModelIntegrationTest::testSearch() {
    m_vm->search(QStringLiteral("computer"));
    QCOMPARE(m_vm->vocabularies().size(), 1);
    QVariantMap voc = m_vm->vocabularies().at(0).toMap();
    QCOMPARE(voc["word"].toString(), QStringLiteral("computer"));
}

QTEST_MAIN(ViewModelIntegrationTest)
#include "ViewModelIntegrationTest.moc"
