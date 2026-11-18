// BridgeController.cpp
// Implementation of BridgeController – the bridge between React frontend
// and C++ backend via Qt WebChannel.

#include "BridgeController.h"
#include "GameSession.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <algorithm>
#include <random>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
BridgeController::BridgeController(QObject *parent) : QObject(parent) {
    // Locate the SQLite database next to the executable.
    const QString dbPath = QCoreApplication::applicationDirPath() + QStringLiteral("/wordsprint.db");
    m_dbMgr = std::make_shared<DatabaseManager>(dbPath);
    m_vocabRepo = std::make_unique<VocabularyRepository>(m_dbMgr);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------
QJsonObject BridgeController::parseJson(const QString &jsonStr) const {
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "BridgeController::parseJson – invalid JSON:" << err.errorString();
        return {};
    }
    return doc.object();
}

QString BridgeController::buildResponse(const QJsonObject &obj) const {
    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

// ---------------------------------------------------------------------------
// Helper: build a JSON error response
// ---------------------------------------------------------------------------
static QString errorResponse(const QString &message) {
    QJsonObject obj;
    obj[QStringLiteral("status")] = QStringLiteral("error");
    obj[QStringLiteral("message")] = message;
    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

// ---------------------------------------------------------------------------
// Helper: convert a Vocabulary struct to a QJsonObject
// (matches the word_pool schema & the WordItem.toJson() contract from Ch. 2)
// ---------------------------------------------------------------------------
static QJsonObject vocabularyToJson(const Vocabulary &v) {
    QJsonObject obj;
    obj[QStringLiteral("wordId")] = v.id;
    obj[QStringLiteral("word")] = QString::fromStdString(v.word);
    obj[QStringLiteral("phonetic")] = QString::fromStdString(v.phonetic);
    obj[QStringLiteral("partOfSpeech")] = QString::fromStdString(v.part_of_speech);
    obj[QStringLiteral("meaningVi")] = QString::fromStdString(v.meaning_vi);
    obj[QStringLiteral("definitionEn")] = QString::fromStdString(v.definition_en);
    obj[QStringLiteral("exampleSentence")] = QString::fromStdString(v.example_sentence);
    obj[QStringLiteral("exampleTranslation")] = QString::fromStdString(v.example_translation);
    obj[QStringLiteral("levelId")] = v.level_id;
    obj[QStringLiteral("topicId")] = v.topic_id;
    return obj;
}

// ---------------------------------------------------------------------------
// startStage
// Sequence diagram step‑by‑step (Chapter 2, §2.5.1):
//   1. React calls startStage({ "stageId": N })
//   2. BridgeController looks up stage config (topic, cefr, timeLimit, questionCount)
//   3. BridgeController fetches random words from word_pool via VocabularyRepository
//   4. Words are serialised to JSON array and returned to React
// ---------------------------------------------------------------------------
QString BridgeController::startStage(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return errorResponse(QStringLiteral("Invalid JSON input"));
    }

    const int stageId = req.value(QStringLiteral("stageId")).toInt(-1);
    if (stageId < 0) {
        return errorResponse(QStringLiteral("Missing or invalid stageId"));
    }

    // --- Step 2: query stage configuration from the stages table -----------
    // Because the existing repository layer does not expose a StageRepository,
    // we query directly via DatabaseManager's underlying QSqlDatabase.
    QSqlQuery stageQuery(m_dbMgr->db());
    stageQuery.prepare(QStringLiteral("SELECT stage_id, topic_id, cefr_level, time_limit, question_count "
                                      "FROM stages WHERE stage_id = :sid"));
    stageQuery.bindValue(QStringLiteral(":sid"), stageId);

    if (!stageQuery.exec() || !stageQuery.next()) {
        qWarning() << "startStage – stage not found:" << stageId;
        return errorResponse(QStringLiteral("Stage not found"));
    }

    const int topicId = stageQuery.value(QStringLiteral("topic_id")).toInt();
    const QString cefrLevel = stageQuery.value(QStringLiteral("cefr_level")).toString();
    const int timeLimit = (stageQuery.value(QStringLiteral("time_limit")).isNull()
                                   ? 60
                                   : stageQuery.value(QStringLiteral("time_limit")).toInt());
    const int questionCount = (stageQuery.value(QStringLiteral("question_count")).isNull()
                                       ? 10
                                       : stageQuery.value(QStringLiteral("question_count")).toInt());

    // --- Step 3: fetch random words from word_pool matching topic & cefr ----
    // Use VocabularyRepository::getVocabulariesFilter with a custom WHERE clause
    // to get words for the given topic and CEFR level.
    const std::string filter = "topic_id = ? AND cefr_level = ? ORDER BY RANDOM() LIMIT ?";
    std::vector<std::string> params;
    params.push_back(std::to_string(topicId));
    params.push_back(cefrLevel.toStdString());
    params.push_back(std::to_string(questionCount));

    std::vector<Vocabulary> words = m_vocabRepo->getVocabulariesFilter(filter, params);

    // Fallback: if the word_pool table does not exist (older schema that uses
    // the `vocabularies` table instead), fall back to getVocabulariesByTopic
    // and limit manually.
    if (words.empty()) {
        words = m_vocabRepo->getVocabulariesByTopic(topicId);
        // Shuffle and trim to questionCount
        std::mt19937 rng(std::random_device{}());
        std::shuffle(words.begin(), words.end(), rng);
        if (static_cast<int>(words.size()) > questionCount) {
            words.resize(static_cast<size_t>(questionCount));
        }
    }

    // --- Step 4: build JSON response ----------------------------------------
    QJsonArray wordsArray;
    std::vector<int> wordIds;
    for (const auto &w: words) {
        wordsArray.append(vocabularyToJson(w));
        wordIds.push_back(w.id);
    }

    // Build stage info object
    QJsonObject stageObj;
    stageObj[QStringLiteral("stageId")] = stageId;
    stageObj[QStringLiteral("topicId")] = topicId;
    stageObj[QStringLiteral("cefrLevel")] = cefrLevel;
    stageObj[QStringLiteral("timeLimit")] = timeLimit;
    stageObj[QStringLiteral("questionCount")] = questionCount;

    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stage")] = stageObj;
    response[QStringLiteral("words")] = wordsArray;

    emit stageStarted(stageId);

    return buildResponse(response);
}

// ---------------------------------------------------------------------------
// completeStage
// Sequence diagram step‑by‑step (Chapter 2, §2.5.2):
//   1. React calls completeStage({ "stageId": N, "userId": U, "score": S, "time": T })
//   2. BridgeController inserts a StageRecord row
//   3. BridgeController updates UserProgress (high_score, best_time, is_completed)
//   4. BridgeController unlocks next stage
//   5. Returns JSON summary to React
// ---------------------------------------------------------------------------
QString BridgeController::completeStage(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return errorResponse(QStringLiteral("Invalid JSON input"));
    }

    const int stageId = req.value(QStringLiteral("stageId")).toInt(-1);
    const int userId = req.value(QStringLiteral("userId")).toInt(1); // default user = 1
    const int score = req.value(QStringLiteral("score")).toInt(0);
    const int completionTime = req.value(QStringLiteral("time")).toInt(0);

    if (stageId < 0) {
        return errorResponse(QStringLiteral("Missing or invalid stageId"));
    }

    // --- Step 2: insert into stage_records ----------------------------------
    QSqlQuery insertRecord(m_dbMgr->db());
    insertRecord.prepare(QStringLiteral("INSERT INTO stage_records (user_id, stage_id, score, completion_time) "
                                        "VALUES (:uid, :sid, :score, :time)"));
    insertRecord.bindValue(QStringLiteral(":uid"), userId);
    insertRecord.bindValue(QStringLiteral(":sid"), stageId);
    insertRecord.bindValue(QStringLiteral(":score"), score);
    insertRecord.bindValue(QStringLiteral(":time"), completionTime);

    int recordId = -1;
    if (!insertRecord.exec()) {
        qWarning() << "completeStage – insertRecord failed:" << insertRecord.lastError().text();
        return errorResponse(QStringLiteral("Failed to save stage record"));
    }
    recordId = insertRecord.lastInsertId().toInt();

    // --- Step 3: update user_progress (upsert) ------------------------------
    QSqlQuery fetchProgress(m_dbMgr->db());
    fetchProgress.prepare(QStringLiteral("SELECT high_score, best_time FROM user_progress "
                                         "WHERE user_id = :uid AND stage_id = :sid"));
    fetchProgress.bindValue(QStringLiteral(":uid"), userId);
    fetchProgress.bindValue(QStringLiteral(":sid"), stageId);

    int highScore = score;
    int bestTime = completionTime;

    if (fetchProgress.exec() && fetchProgress.next()) {
        // Row exists – compare with existing records
        const int existingHighScore = fetchProgress.value(QStringLiteral("high_score")).toInt();
        const int existingBestTime = fetchProgress.value(QStringLiteral("best_time")).toInt();

        highScore = std::max(score, existingHighScore);
        bestTime = (existingBestTime > 0) ? std::min(completionTime, existingBestTime) : completionTime;

        QSqlQuery updateProgress(m_dbMgr->db());
        updateProgress.prepare(
                QStringLiteral("UPDATE user_progress SET is_completed = 1, high_score = :hs, best_time = :bt "
                               "WHERE user_id = :uid AND stage_id = :sid"));
        updateProgress.bindValue(QStringLiteral(":hs"), highScore);
        updateProgress.bindValue(QStringLiteral(":bt"), bestTime);
        updateProgress.bindValue(QStringLiteral(":uid"), userId);
        updateProgress.bindValue(QStringLiteral(":sid"), stageId);
        if (!updateProgress.exec()) {
            qWarning() << "completeStage – updateProgress failed:" << updateProgress.lastError().text();
        }
    } else {
        // No progress row yet – insert one
        QSqlQuery insertProgress(m_dbMgr->db());
        insertProgress.prepare(
                QStringLiteral("INSERT INTO user_progress (user_id, stage_id, is_unlocked, is_completed, "
                               "high_score, best_time) VALUES (:uid, :sid, 1, 1, :hs, :bt)"));
        insertProgress.bindValue(QStringLiteral(":uid"), userId);
        insertProgress.bindValue(QStringLiteral(":sid"), stageId);
        insertProgress.bindValue(QStringLiteral(":hs"), highScore);
        insertProgress.bindValue(QStringLiteral(":bt"), bestTime);
        if (!insertProgress.exec()) {
            qWarning() << "completeStage – insertProgress failed:" << insertProgress.lastError().text();
        }
    }

    // --- Step 4: unlock the next stage --------------------------------------
    QSqlQuery findNext(m_dbMgr->db());
    findNext.prepare(
            QStringLiteral("SELECT stage_id FROM stages "
                           "WHERE unlock_order = (SELECT unlock_order FROM stages WHERE stage_id = :sid) + 1"));
    findNext.bindValue(QStringLiteral(":sid"), stageId);

    bool nextUnlocked = false;
    int nextStageId = -1;
    if (findNext.exec() && findNext.next()) {
        nextStageId = findNext.value(0).toInt();
        QSqlQuery unlockNext(m_dbMgr->db());
        unlockNext.prepare(QStringLiteral("INSERT OR IGNORE INTO user_progress (user_id, stage_id, is_unlocked) "
                                          "VALUES (:uid, :nid, 1)"));
        unlockNext.bindValue(QStringLiteral(":uid"), userId);
        unlockNext.bindValue(QStringLiteral(":nid"), nextStageId);
        if (unlockNext.exec()) {
            nextUnlocked = true;
        }
        // If the row already exists but is_unlocked = 0, update it
        if (!nextUnlocked) {
            QSqlQuery forceUnlock(m_dbMgr->db());
            forceUnlock.prepare(QStringLiteral("UPDATE user_progress SET is_unlocked = 1 "
                                               "WHERE user_id = :uid AND stage_id = :nid"));
            forceUnlock.bindValue(QStringLiteral(":uid"), userId);
            forceUnlock.bindValue(QStringLiteral(":nid"), nextStageId);
            nextUnlocked = forceUnlock.exec();
        }
    }

    // --- Step 5: build JSON response ----------------------------------------
    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("recordId")] = recordId;
    response[QStringLiteral("highScore")] = highScore;
    response[QStringLiteral("bestTime")] = bestTime;
    response[QStringLiteral("nextStageUnlocked")] = nextUnlocked;
    if (nextUnlocked) {
        response[QStringLiteral("nextStageId")] = nextStageId;
    }

    emit stageCompleted(stageId, score);

    return buildResponse(response);
}

// ---------------------------------------------------------------------------
// saveProgress (placeholder – for future extensions)
// ---------------------------------------------------------------------------
QString BridgeController::saveProgress(const QString &requestJson) {
    Q_UNUSED(requestJson);
    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("saveProgress not yet implemented");
    return buildResponse(response);
}

// ---------------------------------------------------------------------------
// loadProgress (placeholder – for future extensions)
// ---------------------------------------------------------------------------
QString BridgeController::loadProgress(const QString &requestJson) {
    Q_UNUSED(requestJson);
    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("loadProgress not yet implemented");
    return buildResponse(response);
}
