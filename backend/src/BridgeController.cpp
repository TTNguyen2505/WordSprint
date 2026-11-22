#include "BridgeController.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>

#include <algorithm>
#include <optional>
#include <vector>

namespace {

QVariant optionalStringValue(const std::optional<std::string>& value) {
    if (!value.has_value()) {
        return {};
    }
    return QString::fromStdString(*value);
}

std::vector<Stage> allStagesForRepository(IVocabularyRepository* repository) {
    std::vector<Stage> stages;
    if (repository == nullptr) {
        return stages;
    }

    const std::vector<Topic> topics = repository->getAllTopics();
    for (const Topic& topic : topics) {
        std::vector<Stage> topicStages = repository->getStagesByTopicId(topic.topic_id);
        stages.insert(stages.end(), topicStages.begin(), topicStages.end());
    }

    std::sort(stages.begin(), stages.end(), [](const Stage& left, const Stage& right) {
        return left.unlock_order < right.unlock_order;
    });
    return stages;
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

QString BridgeController::getAllTopics() {
    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    QVariantList topics;
    for (const Topic& topic : m_repository->getAllTopics()) {
        QVariantMap item;
        item[QStringLiteral("topic_id")] = topic.topic_id;
        item[QStringLiteral("topic_name")] = QString::fromStdString(topic.topic_name);
        item[QStringLiteral("description")] = optionalStringValue(topic.description);
        topics.append(item);
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("topics")] = topics;
    return buildResponse(response);
}

QString BridgeController::getStagesByTopicId(int topicId) {
    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    QVariantList stages;
    for (const Stage& stage : m_repository->getStagesByTopicId(topicId)) {
        QVariantMap item;
        item[QStringLiteral("stage_id")] = stage.stage_id;
        item[QStringLiteral("topic_id")] = stage.topic_id;
        item[QStringLiteral("cefr_level")] = QString::fromStdString(stage.cefr_level);
        item[QStringLiteral("unlock_order")] = stage.unlock_order;
        item[QStringLiteral("time_limit")] = stage.time_limit;
        item[QStringLiteral("question_count")] = stage.question_count;
        stages.append(item);
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stages")] = stages;
    return buildResponse(response);
}

QString BridgeController::startStage(const QString &requestJson) {
    int stageId = 1;
    const QString trimmedRequest = requestJson.trimmed();
    if (!trimmedRequest.isEmpty()) {
        const QJsonObject req = parseJson(trimmedRequest);
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

    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    const std::optional<Stage> stage = m_repository->getStageById(stageId);
    if (!stage.has_value()) {
        return buildErrorResponse(QStringLiteral("Stage was not found"));
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

    const std::optional<Stage> currentStage = m_repository->getStageById(stageId);
    if (!currentStage.has_value()) {
        return buildErrorResponse(QStringLiteral("Stage was not found"));
    }

    const std::optional<int> recordId = m_repository->insertStageRecord(
        userId, stageId, score, completionTime);
    if (!recordId.has_value()) {
        return buildErrorResponse(QStringLiteral("Could not save the stage record"));
    }

    UserProgress progress;
    const std::optional<UserProgress> existingProgress = m_repository->getUserProgress(userId, stageId);
    if (existingProgress.has_value()) {
        progress = *existingProgress;
    } else {
        progress.user_id = userId;
        progress.stage_id = stageId;
        progress.is_unlocked = 1;
    }

    progress.is_completed = 1;
    progress.high_score = std::max(score, progress.high_score);
    if (!progress.best_time.has_value() || *progress.best_time <= 0) {
        progress.best_time = completionTime;
    } else {
        progress.best_time = std::min(completionTime, *progress.best_time);
    }

    if (!m_repository->saveUserProgress(progress)) {
        return buildErrorResponse(QStringLiteral("Could not save user progress"));
    }

    int nextStageId = 0;
    for (const Stage& candidate : allStagesForRepository(m_repository)) {
        if (candidate.unlock_order > currentStage->unlock_order) {
            nextStageId = candidate.stage_id;
            break;
        }
    }

    const bool nextStageUnlocked = nextStageId > 0
        && m_repository->unlockStage(userId, nextStageId);

    QVariantMap response;
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

    // The current schema stores stage-level progress, not per-word answer counts.
    QVariantMap response;
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
    if (req.isEmpty() || !req.contains(QStringLiteral("stagesUnlocked"))) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for saveProgress"));
    }
    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    const int userId = req.value(QStringLiteral("userId")).toInt(1);
    const int stagesUnlocked = req.value(QStringLiteral("stagesUnlocked")).toInt(1);
    if (stagesUnlocked < 1) {
        return buildErrorResponse(QStringLiteral("stagesUnlocked must be positive"));
    }

    const std::vector<Stage> stages = allStagesForRepository(m_repository);
    if (stages.empty()) {
        return buildErrorResponse(QStringLiteral("No stages are available"));
    }

    for (const Stage& stage : stages) {
        if (stage.unlock_order <= stagesUnlocked
            && !m_repository->unlockStage(userId, stage.stage_id)) {
            return buildErrorResponse(QStringLiteral("Could not save unlocked stage progress"));
        }
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("Progress saved successfully");
    return buildResponse(response);
}

QString BridgeController::loadProgress(const QString &requestJson) {
    const QJsonObject req = requestJson.trimmed().isEmpty() ? QJsonObject{} : parseJson(requestJson);
    if (!requestJson.trimmed().isEmpty() && req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for loadProgress"));
    }
    if (m_repository == nullptr) {
        return buildErrorResponse(QStringLiteral("Repository is unavailable"));
    }

    const int userId = req.value(QStringLiteral("userId")).toInt(1);
    const std::vector<Stage> stages = allStagesForRepository(m_repository);
    if (stages.empty()) {
        return buildErrorResponse(QStringLiteral("No stages are available"));
    }

    int stagesUnlocked = 0;
    for (const Stage& stage : stages) {
        const std::optional<UserProgress> progress = m_repository->getUserProgress(userId, stage.stage_id);
        if (progress.has_value() && progress->is_unlocked == 1) {
            stagesUnlocked = std::max(stagesUnlocked, stage.unlock_order);
        }
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stagesUnlocked")] = stagesUnlocked;
    return buildResponse(response);
}
