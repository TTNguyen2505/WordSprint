#include "BridgeController.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <algorithm>

BridgeController::BridgeController(QObject *parent) : QObject(parent) {
    // Ensure default SQLite database connection is available if needed
    if (!QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
        const QString dbPath = QCoreApplication::applicationDirPath() + QStringLiteral("/wordsprint.db");
        db.setDatabaseName(dbPath);
        if (!db.open()) {
            qDebug() << "BridgeController: Default SQLite DB open notice:" << db.lastError().text();
        }
    }
}

QJsonObject BridgeController::parseJson(const QString &jsonStr) const {
    const QString trimmed = jsonStr.trimmed();
    if (trimmed.isEmpty()) {
        return {};
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(trimmed.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "BridgeController::parseJson - parse error:" << error.errorString();
        return {};
    }
    return doc.object();
}

QString BridgeController::buildResponse(const QJsonObject &obj) const {
    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

QString BridgeController::buildErrorResponse(const QString &errorMessage) const {
    QJsonObject obj;
    obj[QStringLiteral("status")] = QStringLiteral("error");
    obj[QStringLiteral("message")] = errorMessage;
    return buildResponse(obj);
}

QJsonArray BridgeController::generateFallbackWords(int stageId, int count) const {
    Q_UNUSED(stageId);
    QJsonArray words;

    struct WordSample {
        const char *word;
        const char *hint;
        const char *cefr;
        const char *image;
        const char *audio;
    };

    static const WordSample samplePool[] = {
        {"APPLE", "A common red or green fruit that keeps the doctor away.", "A1", "assets/quiz/apple.png", "assets/audio/apple.mp3"},
        {"BREAD", "A staple food prepared from dough of flour and water.", "A1", "assets/quiz/bread.png", "assets/audio/bread.mp3"},
        {"CHAIR", "A piece of furniture designed for one person to sit on.", "A1", "assets/quiz/chair.png", "assets/audio/chair.mp3"},
        {"DREAM", "A series of thoughts, images, and sensations occurring in sleep.", "A2", "assets/quiz/dream.png", "assets/audio/dream.mp3"},
        {"EAGLE", "A large bird of prey with massive beak and keen eyesight.", "B1", "assets/quiz/eagle.png", "assets/audio/eagle.mp3"},
        {"FLAME", "A hot glowing body of ignited gas produced by something on fire.", "B1", "assets/quiz/flame.png", "assets/audio/flame.mp3"},
        {"GHOST", "An apparition of a dead person which is believed to appear.", "A2", "assets/quiz/ghost.png", "assets/audio/ghost.mp3"},
        {"HEART", "The hollow muscular organ that pumps blood throughout the body.", "A1", "assets/quiz/heart.png", "assets/audio/heart.mp3"},
        {"ISLAND", "A piece of land completely surrounded by water.", "A2", "assets/quiz/island.png", "assets/audio/island.mp3"},
        {"JUNGLE", "An area of land overgrown with dense forest and tangled vegetation.", "B1", "assets/quiz/jungle.png", "assets/audio/jungle.mp3"}
    };

    const int total = static_cast<int>(sizeof(samplePool) / sizeof(samplePool[0]));
    const int targetCount = std::min(count, total);

    for (int i = 0; i < targetCount; ++i) {
        QJsonObject w;
        w[QStringLiteral("word_id")] = i + 1;
        w[QStringLiteral("target_word")] = QString::fromUtf8(samplePool[i].word);
        w[QStringLiteral("hint_text")] = QString::fromUtf8(samplePool[i].hint);
        w[QStringLiteral("cefr_level")] = QString::fromUtf8(samplePool[i].cefr);
        w[QStringLiteral("image_path")] = QString::fromUtf8(samplePool[i].image);
        w[QStringLiteral("audio_path")] = QString::fromUtf8(samplePool[i].audio);
        w[QStringLiteral("topic_id")] = 1;
        words.append(w);
    }

    return words;
}

QString BridgeController::startStage(const QString &requestJson) {
    int stageId = 1;
    if (!requestJson.trimmed().isEmpty()) {
        const QJsonObject req = parseJson(requestJson);
        if (!req.isEmpty() && req.contains(QStringLiteral("stageId"))) {
            stageId = req.value(QStringLiteral("stageId")).toInt(1);
        } else {
            bool ok = false;
            int directId = requestJson.toInt(&ok);
            if (ok && directId > 0) {
                stageId = directId;
            }
        }
    }

    int timeLimit = 60;
    int questionCount = 10;
    QString cefrLevel = QStringLiteral("A1");
    int topicId = 1;

    // Check database if tables exist
    QSqlDatabase db = QSqlDatabase::database();
    bool queriedFromDb = false;
    QJsonArray wordsArray;

    if (db.isOpen()) {
        QSqlQuery stageQuery;
        stageQuery.prepare(QStringLiteral("SELECT topic_id, cefr_level, time_limit, question_count "
                                          "FROM stages WHERE stage_id = :sid"));
        stageQuery.bindValue(QStringLiteral(":sid"), stageId);
        if (stageQuery.exec() && stageQuery.next()) {
            topicId = stageQuery.value(0).toInt();
            cefrLevel = stageQuery.value(1).toString();
            timeLimit = stageQuery.value(2).toInt();
            questionCount = stageQuery.value(3).toInt();

            QSqlQuery wordQuery;
            wordQuery.prepare(QStringLiteral("SELECT word_id, topic_id, cefr_level, target_word, "
                                             "hint_text, image_path, audio_path FROM word_pool "
                                             "WHERE topic_id = :tid AND cefr_level = :lvl "
                                             "ORDER BY RANDOM() LIMIT :cnt"));
            wordQuery.bindValue(QStringLiteral(":tid"), topicId);
            wordQuery.bindValue(QStringLiteral(":lvl"), cefrLevel);
            wordQuery.bindValue(QStringLiteral(":cnt"), questionCount);

            if (wordQuery.exec()) {
                while (wordQuery.next()) {
                    QJsonObject item;
                    item[QStringLiteral("word_id")] = wordQuery.value(0).toInt();
                    item[QStringLiteral("topic_id")] = wordQuery.value(1).toInt();
                    item[QStringLiteral("cefr_level")] = wordQuery.value(2).toString();
                    item[QStringLiteral("target_word")] = wordQuery.value(3).toString();
                    item[QStringLiteral("hint_text")] = wordQuery.value(4).toString();
                    item[QStringLiteral("image_path")] = wordQuery.value(5).toString();
                    item[QStringLiteral("audio_path")] = wordQuery.value(6).toString();
                    wordsArray.append(item);
                }
                if (!wordsArray.isEmpty()) {
                    queriedFromDb = true;
                }
            }
        }
    }

    // Fallback if DB not ready or returned empty
    if (!queriedFromDb || wordsArray.isEmpty()) {
        wordsArray = generateFallbackWords(stageId, questionCount);
    }

    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stageId")] = stageId;
    response[QStringLiteral("topicId")] = topicId;
    response[QStringLiteral("cefrLevel")] = cefrLevel;
    response[QStringLiteral("timeLimit")] = timeLimit;
    response[QStringLiteral("questionCount")] = wordsArray.size();
    response[QStringLiteral("words")] = wordsArray;

    emit stageStarted(stageId);
    return buildResponse(response);
}

QString BridgeController::completeStage(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for completeStage"));
    }

    const int userId = req.value(QStringLiteral("userId")).toInt(1);
    const int stageId = req.value(QStringLiteral("stageId")).toInt(1);
    const int score = req.value(QStringLiteral("score")).toInt(0);
    const int completionTime = req.value(QStringLiteral("completionTime")).toInt(0);

    int recordId = 1;
    int highScore = score;
    int bestTime = completionTime;
    bool nextStageUnlocked = true;
    int nextStageId = stageId + 1;

    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        // Insert record into stage_records
        QSqlQuery insertQuery;
        insertQuery.prepare(QStringLiteral("INSERT INTO stage_records (user_id, stage_id, score, completion_time) "
                                           "VALUES (:uid, :sid, :sc, :ct)"));
        insertQuery.bindValue(QStringLiteral(":uid"), userId);
        insertQuery.bindValue(QStringLiteral(":sid"), stageId);
        insertQuery.bindValue(QStringLiteral(":sc"), score);
        insertQuery.bindValue(QStringLiteral(":ct"), completionTime);
        if (insertQuery.exec()) {
            recordId = insertQuery.lastInsertId().toInt();
        }

        // Query existing progress
        QSqlQuery progressQuery;
        progressQuery.prepare(QStringLiteral("SELECT high_score, best_time FROM user_progress "
                                             "WHERE user_id = :uid AND stage_id = :sid"));
        progressQuery.bindValue(QStringLiteral(":uid"), userId);
        progressQuery.bindValue(QStringLiteral(":sid"), stageId);
        if (progressQuery.exec() && progressQuery.next()) {
            const int prevHigh = progressQuery.value(0).toInt();
            const int prevBest = progressQuery.value(1).toInt();
            highScore = std::max(score, prevHigh);
            bestTime = (prevBest > 0) ? std::min(completionTime, prevBest) : completionTime;

            QSqlQuery updateQuery;
            updateQuery.prepare(QStringLiteral("UPDATE user_progress SET is_completed = 1, high_score = :hs, "
                                               "best_time = :bt WHERE user_id = :uid AND stage_id = :sid"));
            updateQuery.bindValue(QStringLiteral(":hs"), highScore);
            updateQuery.bindValue(QStringLiteral(":bt"), bestTime);
            updateQuery.bindValue(QStringLiteral(":uid"), userId);
            updateQuery.bindValue(QStringLiteral(":sid"), stageId);
            updateQuery.exec();
        } else {
            QSqlQuery insertProgress;
            insertProgress.prepare(QStringLiteral("INSERT INTO user_progress (user_id, stage_id, is_unlocked, "
                                                  "is_completed, high_score, best_time) "
                                                  "VALUES (:uid, :sid, 1, 1, :hs, :bt)"));
            insertProgress.bindValue(QStringLiteral(":uid"), userId);
            insertProgress.bindValue(QStringLiteral(":sid"), stageId);
            insertProgress.bindValue(QStringLiteral(":hs"), highScore);
            insertProgress.bindValue(QStringLiteral(":bt"), bestTime);
            insertProgress.exec();
        }

        // Unlock next stage
        QSqlQuery unlockQuery;
        unlockQuery.prepare(QStringLiteral("INSERT OR REPLACE INTO user_progress (user_id, stage_id, is_unlocked) "
                                           "VALUES (:uid, :nid, 1)"));
        unlockQuery.bindValue(QStringLiteral(":uid"), userId);
        unlockQuery.bindValue(QStringLiteral(":nid"), nextStageId);
        unlockQuery.exec();
    }

    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("recordId")] = recordId;
    response[QStringLiteral("highScore")] = highScore;
    response[QStringLiteral("bestTime")] = bestTime;
    response[QStringLiteral("nextStageUnlocked")] = nextStageUnlocked;
    response[QStringLiteral("nextStageId")] = nextStageId;

    emit stageCompleted(stageId, score);
    return buildResponse(response);
}

QString BridgeController::processAnswer(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for processAnswer"));
    }

    QJsonObject payload = req;
    if (req.contains(QStringLiteral("payload")) && req.value(QStringLiteral("payload")).isObject()) {
        payload = req.value(QStringLiteral("payload")).toObject();
    }

    const QString answer = payload.value(QStringLiteral("answer")).toString().trimmed();
    const QString expected = payload.value(QStringLiteral("expected")).toString().trimmed();

    bool isCorrect = false;
    if (!expected.isEmpty()) {
        isCorrect = (answer.compare(expected, Qt::CaseInsensitive) == 0);
    } else {
        isCorrect = !answer.isEmpty();
    }

    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("correct")] = isCorrect;
    response[QStringLiteral("score")] = isCorrect ? 100 : 0;
    response[QStringLiteral("timeRemaining")] = payload.value(QStringLiteral("timeRemaining")).toDouble(60.0);

    return buildResponse(response);
}

QString BridgeController::processMessage(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Empty or invalid JSON message"));
    }

    const QString action = req.value(QStringLiteral("action")).toString();
    if (action == QStringLiteral("startStage") || action == QStringLiteral("startGame")) {
        return startStage(requestJson);
    }
    if (action == QStringLiteral("completeStage") || action == QStringLiteral("endGame")) {
        return completeStage(requestJson);
    }
    if (action == QStringLiteral("processAnswer")) {
        return processAnswer(requestJson);
    }

    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("Action processed: ") + action;
    return buildResponse(response);
}

QString BridgeController::saveProgress(const QString &requestJson) {
    Q_UNUSED(requestJson);
    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("Progress saved successfully");
    return buildResponse(response);
}

QString BridgeController::loadProgress(const QString &requestJson) {
    Q_UNUSED(requestJson);
    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stagesUnlocked")] = 1;
    return buildResponse(response);
}
