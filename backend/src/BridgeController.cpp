#include "BridgeController.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

#include <algorithm>
#include <optional>
#include <vector>

namespace {

QString optionalStringValue(const std::optional<std::string>& value) {
    return value.has_value() ? QString::fromStdString(*value) : QString();
}

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
    const QString trimmedRequest = requestJson.trimmed();
    if (!trimmedRequest.isEmpty()) {
        req = parseJson(trimmedRequest);
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

    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    std::vector<Vocabulary> vocabularies = m_repository->getRandomVocabularies(
        questionCount, topicId, cefrLevel.toStdString());
    if (vocabularies.empty()) {
        vocabularies = m_repository->getRandomVocabularies(questionCount);
    }
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
    response[QStringLiteral("stageId")] = stageId;
    response[QStringLiteral("topicId")] = topicId;
    response[QStringLiteral("cefrLevel")] = cefrLevel;
    response[QStringLiteral("timeLimit")] = timeLimit;
    response[QStringLiteral("questionCount")] = words.size();
    response[QStringLiteral("words")] = words;

    emit stageStarted(stageId);
    return buildResponse(response);
}

QString BridgeController::getAllTopics() {
    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    QVariantList topics;
    for (const Topic& topic : m_repository->getAllTopics()) {
        QVariantMap item;
        item[QStringLiteral("id")] = topic.id;
        item[QStringLiteral("name")] = QString::fromStdString(topic.name);
        item[QStringLiteral("description")] = QString::fromStdString(topic.description);
        topics.append(item);
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("topics")] = topics;
    return buildResponse(response);
}

QString BridgeController::getStagesByTopicId(int topicId) {
    Q_UNUSED(topicId);
    return buildErrorResponse(QStringLiteral("Stage lookup is unavailable in IVocabularyRepository"));
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

    QVariantMap response;
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
    // TODO(OOP-34): Delegate scoring to GameSession & BasePuzzle (ScramblePuzzle, FillInPuzzle, AudioPuzzle) once merged to main
    const auto buildAnswerResponse = [this](
        bool isValid,
        bool isCorrect,
        int score,
        const QString& correctAnswer,
        const QString& message,
        const QString& explanation,
        double timeRemaining) {
        QVariantMap response;
        response[QStringLiteral("status")] = isValid ? QStringLiteral("ok") : QStringLiteral("error");
        response[QStringLiteral("correct")] = isCorrect;
        response[QStringLiteral("score")] = score;
        response[QStringLiteral("correctAnswer")] = correctAnswer;
        response[QStringLiteral("message")] = message;
        response[QStringLiteral("explanation")] = explanation;
        response[QStringLiteral("timeRemaining")] = timeRemaining;
        return buildResponse(response);
    };

    constexpr double defaultTimeRemaining = 60.0;
    const QString invalidRequestMessage = QStringLiteral("Invalid JSON payload for processAnswer");
    const QString invalidRequestExplanation = QStringLiteral(
        "Provide a JSON object with non-empty string fields 'answer' and 'expected'.");
    if (requestJson.trimmed().isEmpty()) {
        return buildAnswerResponse(
            false, false, 0, QString(), invalidRequestMessage, invalidRequestExplanation, defaultTimeRemaining);
    }

    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildAnswerResponse(
            false, false, 0, QString(), invalidRequestMessage, invalidRequestExplanation, defaultTimeRemaining);
    }

    QJsonObject payload = req;
    const bool hasNestedPayload = req.contains(QStringLiteral("payload"));
    if (hasNestedPayload && !req.value(QStringLiteral("payload")).isObject()) {
        return buildAnswerResponse(
            false, false, 0, QString(), invalidRequestMessage, invalidRequestExplanation, defaultTimeRemaining);
    }
    if (hasNestedPayload) {
        payload = req.value(QStringLiteral("payload")).toObject();
    }

    const QJsonValue answerValue = payload.value(QStringLiteral("answer"));
    const QJsonValue expectedValue = payload.value(QStringLiteral("expected"));
    if (!answerValue.isString() || !expectedValue.isString()) {
        return buildAnswerResponse(
            false, false, 0, QString(), invalidRequestMessage, invalidRequestExplanation, defaultTimeRemaining);
    }

    const QString answer = answerValue.toString().trimmed();
    const QString expected = expectedValue.toString().trimmed();
    if (answer.isEmpty() || expected.isEmpty()) {
        return buildAnswerResponse(
            false, false, 0, expected, invalidRequestMessage, invalidRequestExplanation, defaultTimeRemaining);
    }

    const bool isCorrect = answer.compare(expected, Qt::CaseInsensitive) == 0;
    const double timeRemaining = payload.value(QStringLiteral("timeRemaining")).toDouble(defaultTimeRemaining);

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

    const QString message = isCorrect ? QStringLiteral("Correct answer") : QStringLiteral("Incorrect answer");
    const QString explanation = isCorrect
        ? QStringLiteral("The submitted answer matches the expected answer.")
        : QStringLiteral("The correct answer is '%1'.").arg(expected);
    return buildAnswerResponse(
        true, isCorrect, isCorrect ? 100 : 0, expected, message, explanation, timeRemaining);
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

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("Progress saved successfully");
    return buildResponse(response);
}

QString BridgeController::loadProgress(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    const int userId = req.value(QStringLiteral("userId")).toInt(1);
    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stagesUnlocked")] = m_unlockedStagesByUser.value(userId, 1);
    return buildResponse(response);
}
