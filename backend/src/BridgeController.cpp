#include "BridgeController.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

#include <algorithm>
#include <optional>
#include <vector>

BridgeController::BridgeController(IVocabularyRepository* repository, QObject *parent)
    : QObject(parent), m_repository(repository) {}

}  // namespace

BridgeController::BridgeController(IVocabularyRepository* repository, QObject *parent)
    : QObject(parent), m_repository(repository) {}

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

QString BridgeController::buildResponse(const QVariantMap &obj) const {
    return QString::fromUtf8(QJsonDocument::fromVariant(obj).toJson(QJsonDocument::Compact));
}

QString BridgeController::buildErrorResponse(const QString &errorMessage) const {
    QVariantMap obj;
    obj[QStringLiteral("status")] = QStringLiteral("error");
    obj[QStringLiteral("message")] = errorMessage;
    return buildResponse(obj);
}

QString BridgeController::startStage(const QString &requestJson) {
    int stageId = 1;
    QJsonObject req;
    if (!requestJson.trimmed().isEmpty()) {
        req = parseJson(requestJson);
        if (!req.isEmpty() && req.contains(QStringLiteral("stageId"))) {
            stageId = req.value(QStringLiteral("stageId")).toInt(1);
        } else {
            bool ok = false;
            const int directId = trimmedRequest.toInt(&ok);
            if (ok && directId > 0) {
                stageId = directId;
            }
        }
    }

    int timeLimit = 60;
    int questionCount = 10;
    QString cefrLevel = QStringLiteral("A1");
    int topicId = 1;
    if (!req.isEmpty()) {
        timeLimit = req.value(QStringLiteral("timeLimit")).toInt(timeLimit);
        questionCount = req.value(QStringLiteral("questionCount")).toInt(questionCount);
        cefrLevel = req.value(QStringLiteral("cefrLevel")).toString(cefrLevel);
        topicId = req.value(QStringLiteral("topicId")).toInt(topicId);
    }

    QJsonArray wordsArray;
    if (m_repository != nullptr) {
        std::vector<Vocabulary> vocabularies = m_repository->getRandomVocabularies(
            questionCount, topicId, cefrLevel.toStdString());
        if (vocabularies.empty()) {
            vocabularies = m_repository->getRandomVocabularies(questionCount);
        }

        for (const Vocabulary& vocabulary : vocabularies) {
            QJsonObject item;
            item[QStringLiteral("word_id")] = vocabulary.id;
            item[QStringLiteral("topic_id")] = vocabulary.topicId;
            item[QStringLiteral("cefr_level")] = QString::fromStdString(vocabulary.cefrLevel);
            item[QStringLiteral("target_word")] = QString::fromStdString(vocabulary.word);
            item[QStringLiteral("hint_text")] = QString();
            item[QStringLiteral("image_path")] = QString();
            item[QStringLiteral("audio_path")] = QString();
            wordsArray.append(item);
        }
    }

    const std::vector<Vocabulary> vocabularies = m_repository->getRandomVocabularies(
        stage->question_count, stage->topic_id, stage->cefr_level);
    if (vocabularies.empty()) {
        return buildErrorResponse(QStringLiteral("No vocabulary is available for this stage"));
    }

    QVariantList words;
    for (const Vocabulary& vocabulary : vocabularies) {
        QVariantMap item;
        item[QStringLiteral("word_id")] = vocabulary.word_id;
        item[QStringLiteral("topic_id")] = vocabulary.topic_id;
        item[QStringLiteral("cefr_level")] = QString::fromStdString(vocabulary.cefr_level);
        item[QStringLiteral("target_word")] = QString::fromStdString(vocabulary.target_word);
        item[QStringLiteral("hint_text")] = optionalStringValue(vocabulary.hint_text);
        item[QStringLiteral("image_path")] = optionalStringValue(vocabulary.image_path);
        item[QStringLiteral("audio_path")] = QString::fromStdString(vocabulary.audio_path);
        words.append(item);
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stageId")] = stage->stage_id;
    response[QStringLiteral("topicId")] = stage->topic_id;
    response[QStringLiteral("cefrLevel")] = QString::fromStdString(stage->cefr_level);
    response[QStringLiteral("timeLimit")] = stage->time_limit;
    response[QStringLiteral("questionCount")] = words.size();
    response[QStringLiteral("words")] = words;

    emit stageStarted(stage->stage_id);
    return buildResponse(response);
}

QString BridgeController::completeStage(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for completeStage"));
    }
    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    const int userId = req.value(QStringLiteral("userId")).toInt(1);
    const int stageId = req.value(QStringLiteral("stageId")).toInt(1);
    const int score = req.value(QStringLiteral("score")).toInt(0);
    const int completionTime = req.value(QStringLiteral("completionTime")).toInt(0);

    int recordId = m_nextRecordId++;
    int highScore = score;
    int bestTime = completionTime;
    bool nextStageUnlocked = true;
    int nextStageId = stageId + 1;

    const QString progressKey = QStringLiteral("%1:%2").arg(userId).arg(stageId);
    const auto existingProgress = m_stageProgress.constFind(progressKey);
    if (existingProgress != m_stageProgress.cend()) {
        highScore = std::max(score, existingProgress->highScore);
        bestTime = existingProgress->bestTime > 0
            ? std::min(completionTime, existingProgress->bestTime)
            : completionTime;
    }

    m_stageProgress.insert(progressKey, StageProgress{recordId, highScore, bestTime});
    m_unlockedStagesByUser.insert(
        userId, std::max(m_unlockedStagesByUser.value(userId, 1), nextStageId));

    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("recordId")] = *recordId;
    response[QStringLiteral("highScore")] = progress.high_score;
    response[QStringLiteral("bestTime")] = progress.best_time.value_or(completionTime);
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
    const bool hasNestedPayload = req.contains(QStringLiteral("payload"))
        && req.value(QStringLiteral("payload")).isObject();
    if (hasNestedPayload) {
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

    const auto vocabIdFrom = [](const QJsonObject& object) {
        const QStringList idKeys = {QStringLiteral("vocabId"), QStringLiteral("wordId"), QStringLiteral("id")};
        for (const QString& key : idKeys) {
            const int vocabId = object.value(key).toInt();
            if (vocabId > 0) {
                return vocabId;
            }
        }
        return 0;
    };
    int vocabId = vocabIdFrom(payload);
    if (vocabId <= 0 && hasNestedPayload) {
        vocabId = vocabIdFrom(req);
    }
    if (vocabId > 0 && m_repository != nullptr) {
        m_repository->updateLearningProgress(vocabId, isCorrect);
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
    if (action == QStringLiteral("getAllTopics")) {
        return getAllTopics();
    }
    if (action == QStringLiteral("getStagesByTopicId")) {
        return getStagesByTopicId(req.value(QStringLiteral("topicId")).toInt());
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("Action processed: ") + action;
    return buildResponse(response);
}

QString BridgeController::saveProgress(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (!req.isEmpty() && req.contains(QStringLiteral("stagesUnlocked"))) {
        const int userId = req.value(QStringLiteral("userId")).toInt(1);
        const int stagesUnlocked = req.value(QStringLiteral("stagesUnlocked")).toInt(1);
        if (stagesUnlocked > 0) {
            m_unlockedStagesByUser.insert(
                userId, std::max(m_unlockedStagesByUser.value(userId, 1), stagesUnlocked));
        }
    }

    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("Progress saved successfully");
    return buildResponse(response);
}

QString BridgeController::loadProgress(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    const int userId = req.value(QStringLiteral("userId")).toInt(1);
    QJsonObject response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stagesUnlocked")] = m_unlockedStagesByUser.value(userId, 1);
    return buildResponse(response);
}
