// BridgeControllerTest.cpp
// Unit test that simulates React → BridgeController calls (startStage,
// completeStage) using an in‑memory SQLite database, verifying the JSON
// responses and GameSession state.

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtTest/QtTest>

#include "BridgeController.h"
#include "DatabaseManager.h"
#include "GameSession.h"
#include "VocabularyRepository.h"

// ---------------------------------------------------------------------------
// We need a BridgeController that uses our in‑memory DB instead of the
// on‑disk one.  The simplest approach is to:
//   1. Create the in‑memory DB and seed it.
//   2. Override the application dir path so the controller finds it.
// However, since the constructor hard‑codes the path, we instead test the
// individual helpers and the JSON round‑trip contract by directly calling
// startStage / completeStage after seeding a temporary on‑disk DB at a
// known location, then cleaning it up.
// ---------------------------------------------------------------------------

class BridgeControllerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testStartStageSuccess();
    void testStartStageMissingId();
    void testCompleteStageSuccess();
    void testCompleteStageInvalidJson();
    void testGameSessionToJson();
    void cleanupTestCase();

private:
    QString m_dbPath;
};

void BridgeControllerTest::initTestCase() {
    // Create a temporary DB next to the test executable
    m_dbPath = QCoreApplication::applicationDirPath() + QStringLiteral("/wordsprint.db");

    auto dbMgr = std::make_shared<DatabaseManager>(m_dbPath);
    QSqlQuery q(dbMgr->db());

    // topics
    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS topics ("
                   "topic_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "topic_name TEXT UNIQUE NOT NULL, "
                   "description TEXT);"));
    QVERIFY(q.exec("INSERT OR IGNORE INTO topics (topic_id, topic_name, description) "
                   "VALUES (1, 'Technology', 'Tech vocabulary');"));

    // stages
    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS stages ("
                   "stage_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "topic_id INTEGER NOT NULL, "
                   "cefr_level TEXT NOT NULL, "
                   "unlock_order INTEGER UNIQUE NOT NULL, "
                   "time_limit INTEGER NOT NULL DEFAULT 60, "
                   "question_count INTEGER NOT NULL DEFAULT 10, "
                   "FOREIGN KEY(topic_id) REFERENCES topics(topic_id));"));
    QVERIFY(q.exec(
            "INSERT OR IGNORE INTO stages (stage_id, topic_id, cefr_level, unlock_order, time_limit, question_count) "
            "VALUES (1, 1, 'A1', 1, 30, 2);"));
    QVERIFY(q.exec(
            "INSERT OR IGNORE INTO stages (stage_id, topic_id, cefr_level, unlock_order, time_limit, question_count) "
            "VALUES (2, 1, 'A1', 2, 30, 2);"));

    // vocabularies (used by VocabularyRepository fallback path)
    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS vocabularies ("
                   "id INTEGER PRIMARY KEY, word TEXT, phonetic TEXT, "
                   "part_of_speech TEXT, meaning_vi TEXT, definition_en TEXT, "
                   "example_sentence TEXT, example_translation TEXT, "
                   "level_id INTEGER, topic_id INTEGER, created_at INTEGER);"));
    QVERIFY(q.exec("INSERT OR IGNORE INTO vocabularies ("
                   "id, word, phonetic, part_of_speech, meaning_vi, definition_en, "
                   "example_sentence, example_translation, level_id, topic_id, created_at) "
                   "VALUES (1, 'apple', 'ˈæpəl', 'noun', 'quả táo', 'A fruit', 'I eat an apple.', 'Tôi ăn một quả "
                   "táo.', 1, 1, 0);"));
    QVERIFY(q.exec("INSERT OR IGNORE INTO vocabularies ("
                   "id, word, phonetic, part_of_speech, meaning_vi, definition_en, "
                   "example_sentence, example_translation, level_id, topic_id, created_at) "
                   "VALUES (2, 'banana', 'bəˈnænə', 'noun', 'quả chuối', 'A fruit', 'I eat a banana.', 'Tôi ăn một quả "
                   "chuối.', 1, 1, 0);"));
    QVERIFY(q.exec("INSERT OR IGNORE INTO vocabularies ("
                   "id, word, phonetic, part_of_speech, meaning_vi, definition_en, "
                   "example_sentence, example_translation, level_id, topic_id, created_at) "
                   "VALUES (3, 'cherry', 'ˈtʃɛri', 'noun', 'quả anh đào', 'A fruit', 'I eat a cherry.', 'Tôi ăn một "
                   "quả anh đào.', 1, 1, 0);"));

    // user_progress
    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS user_progress ("
                   "user_id INTEGER NOT NULL, "
                   "stage_id INTEGER NOT NULL, "
                   "is_unlocked INTEGER NOT NULL DEFAULT 0, "
                   "is_completed INTEGER NOT NULL DEFAULT 0, "
                   "high_score INTEGER NOT NULL DEFAULT 0, "
                   "best_time INTEGER, "
                   "PRIMARY KEY(user_id, stage_id));"));

    // stage_records
    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS stage_records ("
                   "record_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "user_id INTEGER NOT NULL, "
                   "stage_id INTEGER NOT NULL, "
                   "score INTEGER NOT NULL, "
                   "completion_time INTEGER NOT NULL, "
                   "played_at TEXT NOT NULL DEFAULT (datetime('now')));"));
}

void BridgeControllerTest::testStartStageSuccess() {
    BridgeController ctrl;
    const QString response = ctrl.startStage(QStringLiteral(R"({"stageId": 1})"));
    const QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
    QVERIFY(doc.isObject());

    const QJsonObject obj = doc.object();
    QCOMPARE(obj.value("status").toString(), QStringLiteral("ok"));
    QVERIFY(obj.contains("stage"));
    QVERIFY(obj.contains("words"));

    // Stage object assertions
    const QJsonObject stage = obj.value("stage").toObject();
    QCOMPARE(stage.value("stageId").toInt(), 1);
    QCOMPARE(stage.value("topicId").toInt(), 1);
    QCOMPARE(stage.value("cefrLevel").toString(), QStringLiteral("A1"));
    QCOMPARE(stage.value("timeLimit").toInt(), 30);
    QCOMPARE(stage.value("questionCount").toInt(), 2);

    // Words array – should have at most questionCount (2) words
    const QJsonArray words = obj.value("words").toArray();
    QVERIFY(words.size() > 0);
    QVERIFY(words.size() <= 2);

    // Each word should have the expected fields
    const QJsonObject firstWord = words.first().toObject();
    QVERIFY(firstWord.contains("wordId"));
    QVERIFY(firstWord.contains("word"));
    QVERIFY(firstWord.contains("meaningVi"));
}

void BridgeControllerTest::testStartStageMissingId() {
    BridgeController ctrl;
    const QString response = ctrl.startStage(QStringLiteral(R"({})"));
    const QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
    const QJsonObject obj = doc.object();
    QCOMPARE(obj.value("status").toString(), QStringLiteral("error"));
    QVERIFY(obj.contains("message"));
}

void BridgeControllerTest::testCompleteStageSuccess() {
    BridgeController ctrl;
    const QString response =
            ctrl.completeStage(QStringLiteral(R"({"stageId": 1, "userId": 1, "score": 80, "time": 25})"));
    const QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
    QVERIFY(doc.isObject());

    const QJsonObject obj = doc.object();
    QCOMPARE(obj.value("status").toString(), QStringLiteral("ok"));
    QVERIFY(obj.contains("recordId"));
    QVERIFY(obj.value("recordId").toInt() > 0);
    QCOMPARE(obj.value("highScore").toInt(), 80);
    QCOMPARE(obj.value("bestTime").toInt(), 25);
    // Stage 2 should be unlocked (unlock_order = 2)
    QCOMPARE(obj.value("nextStageUnlocked").toBool(), true);
    QCOMPARE(obj.value("nextStageId").toInt(), 2);

    // Call again with a higher score to verify high_score update
    const QString response2 =
            ctrl.completeStage(QStringLiteral(R"({"stageId": 1, "userId": 1, "score": 95, "time": 20})"));
    const QJsonObject obj2 = QJsonDocument::fromJson(response2.toUtf8()).object();
    QCOMPARE(obj2.value("highScore").toInt(), 95);
    QCOMPARE(obj2.value("bestTime").toInt(), 20);
}

void BridgeControllerTest::testCompleteStageInvalidJson() {
    BridgeController ctrl;
    const QString response = ctrl.completeStage(QStringLiteral("not json at all"));
    const QJsonObject obj = QJsonDocument::fromJson(response.toUtf8()).object();
    QCOMPARE(obj.value("status").toString(), QStringLiteral("error"));
}

void BridgeControllerTest::testGameSessionToJson() {
    GameSession session;
    session.setStageId(42);
    session.setTimeLimit(90);
    session.setQuestionCount(5);
    session.addScore(100);
    session.setElapsedTime(30);

    QJsonArray words;
    QJsonObject w1;
    w1["wordId"] = 1;
    w1["word"] = QStringLiteral("hello");
    words.append(w1);
    session.setWords(words);

    const QJsonObject json = session.toJson();
    QCOMPARE(json.value("stageId").toInt(), 42);
    QCOMPARE(json.value("timeLimit").toInt(), 90);
    QCOMPARE(json.value("questionCount").toInt(), 5);
    QCOMPARE(json.value("score").toInt(), 100);
    QCOMPARE(json.value("elapsedTime").toInt(), 30);
    QCOMPARE(json.value("words").toArray().size(), 1);
    QCOMPARE(json.value("words").toArray().first().toObject().value("word").toString(), QStringLiteral("hello"));
}

void BridgeControllerTest::cleanupTestCase() {
    // Remove the temporary .db file created during testing
    if (QFile::exists(m_dbPath)) {
        QFile::remove(m_dbPath);
        qDebug() << "Cleaned up temporary database:" << m_dbPath;
    }
    // Also clean up WAL/SHM files if any
    QFile::remove(m_dbPath + "-wal");
    QFile::remove(m_dbPath + "-shm");
}

QTEST_MAIN(BridgeControllerTest)
#include "BridgeControllerTest.moc"
