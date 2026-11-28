#include "BridgeController.h"

#include "models/CEFRLevel.h"
#include "models/StageMapItem.h"
#include "models/StageRecordDTO.h"
#include "models/StageStatusDTO.h"
#include "models/Topic.h"
#include "models/WordItem.h"
#include "repositories/RecordRepository.h"
#include "repositories/StageRepository.h"
#include "repositories/UserRepository.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>

#include <algorithm>
#include <map>
#include <optional>
#include <vector>

BridgeController::BridgeController(StageRepository *stageRepository, RecordRepository *recordRepository,
                                   UserRepository *userRepository, QObject *parent) :
    QObject(parent), m_stageRepository(stageRepository), m_recordRepository(recordRepository),
    m_userRepository(userRepository) {}

namespace {
    struct AudioFallbackResource {
        QString url;
        QString status;
    };

    AudioFallbackResource audioFallbackResource(const std::optional<std::string> &audioPath) {
        if (!audioPath.has_value() || audioPath->empty()) {
            return {{}, QStringLiteral("missing")};
        }

        QString path = QString::fromStdString(*audioPath).trimmed();
        path.replace(QLatin1Char('\\'), QLatin1Char('/'));
        const QStringList pathParts = path.split(QLatin1Char('/'));
        if (!path.startsWith(QStringLiteral("storage/audio/")) || pathParts.contains(QStringLiteral(".")) ||
            pathParts.contains(QStringLiteral(".."))) {
            return {{}, QStringLiteral("invalid_path")};
        }

        if (!QFile::exists(QStringLiteral(":/") + path)) {
            return {{}, QStringLiteral("not_found")};
        }

        QUrl resourceUrl;
        resourceUrl.setScheme(QStringLiteral("qrc"));
        resourceUrl.setPath(QStringLiteral("/") + path);
        return {resourceUrl.toString(QUrl::FullyEncoded), QStringLiteral("available")};
    }

    QVariantMap userToVariant(const User &user) {
        return {{QStringLiteral("id"), user.user_id},
                {QStringLiteral("name"), QString::fromStdString(user.username)},
                {QStringLiteral("musicEnabled"), user.bgm_enabled},
                {QStringLiteral("soundEnabled"), user.sfx_enabled},
                {QStringLiteral("musicVolume"), user.bgm_volume},
                {QStringLiteral("soundVolume"), user.sfx_volume}};
    }

    QVariantMap stageMapToVariant(int userId, const std::vector<Topic> &topics,
                                  const std::vector<StageMapItem> &stageItems) {
        std::map<int, QVariantList> stagesByTopic;
        for (const StageMapItem &stage: stageItems) {
            stagesByTopic[stage.topic_id].append(
                    QVariantMap{{QStringLiteral("stageId"), stage.stage_id},
                                {QStringLiteral("topicId"), stage.topic_id},
                                {QStringLiteral("topicName"), QString::fromStdString(stage.topic_name)},
                                {QStringLiteral("cefrLevel"), QString::fromStdString(stage.cefr_level)},
                                {QStringLiteral("questionCount"), stage.question_count},
                                {QStringLiteral("timeLimit"), stage.time_limit},
                                {QStringLiteral("passScore"), stage.pass_score},
                                {QStringLiteral("isUnlocked"), stage.is_unlocked},
                                {QStringLiteral("isPassed"), stage.is_passed},
                                {QStringLiteral("bestScore"), stage.best_score},
                                {QStringLiteral("bestTime"), stage.best_time}});
        }

        QVariantList topicList;
        for (const Topic &topic: topics) {
            QVariantMap topicItem;
            topicItem[QStringLiteral("topicId")] = topic.id;
            topicItem[QStringLiteral("topicName")] = QString::fromStdString(topic.name);
            topicItem[QStringLiteral("stages")] = stagesByTopic[topic.id];
            topicList.append(topicItem);
        }

        const QVariantList cefrLevels = {QStringLiteral("A1"), QStringLiteral("A2"), QStringLiteral("B1"),
                                         QStringLiteral("B2"), QStringLiteral("C1"), QStringLiteral("C2")};
        return {{QStringLiteral("status"), QStringLiteral("ok")},
                {QStringLiteral("userId"), userId},
                {QStringLiteral("cefrLevels"), cefrLevels},
                {QStringLiteral("topics"), topicList}};
    }
} // namespace

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
    if (m_stageRepository == nullptr) {
        return buildErrorResponse(QStringLiteral("Stage repository is unavailable"));
    }

    const QJsonObject request = parseJson(requestJson);
    if (request.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for startStage"));
    }
    const QJsonObject payload = request.contains(QStringLiteral("payload")) &&
                                                request.value(QStringLiteral("payload")).isObject()
                                        ? request.value(QStringLiteral("payload")).toObject()
                                        : request;
    const int userId = payload.value(QStringLiteral("userId")).toInt();
    const int stageId = payload.value(QStringLiteral("stageId")).toInt();
    if (userId <= 0 || stageId <= 0) {
        return buildErrorResponse(QStringLiteral("A valid userId and stageId are required"));
    }

    const auto stageMap = m_stageRepository->getStageMapForUser(userId);
    if (!stageMap.has_value()) {
        return buildErrorResponse(QStringLiteral("Could not load stage map for user"));
    }
    const auto selectedStage = std::find_if(stageMap->begin(), stageMap->end(), [stageId](const StageMapItem &item) {
        return item.stage_id == stageId;
    });
    if (selectedStage == stageMap->end()) {
        return buildErrorResponse(QStringLiteral("Stage was not found"));
    }
    if (!selectedStage->is_unlocked) {
        return buildErrorResponse(QStringLiteral("Stage is locked and cannot be started"));
    }

    const std::optional<Stage> stage = m_stageRepository->getStageById(stageId);
    if (!stage.has_value()) {
        return buildErrorResponse(QStringLiteral("Stage was not found"));
    }

    const std::optional<CEFRLevel> level = cefrLevelFromName(stage->cefr_level);
    if (!level.has_value()) {
        return buildErrorResponse(QStringLiteral("Stage has an unsupported CEFR level"));
    }

    const std::vector<WordItem> wordItems =
            m_stageRepository->getRandomWords(stage->topic_id, *level, stage->question_count);
    if (wordItems.empty()) {
        return buildErrorResponse(QStringLiteral("No vocabulary is available for this stage"));
    }
    if (static_cast<int>(wordItems.size()) < stage->question_count) {
        return buildErrorResponse(QStringLiteral("Not enough vocabulary with valid target words is available"));
    }

    QVariantList words;
    for (const WordItem &word: wordItems) {
        QVariantMap item;
        item[QStringLiteral("word_id")] = word.word_id;
        item[QStringLiteral("topic_id")] = word.topic_id;
        item[QStringLiteral("cefr_level")] = QString::fromStdString(word.cefr_level);
        item[QStringLiteral("target_word")] = QString::fromStdString(word.target_word);
        item[QStringLiteral("hint_text")] = QString();
        item[QStringLiteral("image_path")] = QString::fromStdString(word.hint_image_path);
        const AudioFallbackResource audioFallback = audioFallbackResource(word.audio_path);
        item[QStringLiteral("audioFallbackUrl")] = audioFallback.url;
        item[QStringLiteral("audioFallbackStatus")] = audioFallback.status;
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

QString BridgeController::getStageMap(const QString &requestJson) {
    if (m_stageRepository == nullptr) {
        return buildErrorResponse(QStringLiteral("Stage repository is unavailable"));
    }

    const QJsonObject request = parseJson(requestJson);
    if (request.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for getStageMap"));
    }
    const QJsonObject payload = request.contains(QStringLiteral("payload")) &&
                                                request.value(QStringLiteral("payload")).isObject()
                                        ? request.value(QStringLiteral("payload")).toObject()
                                        : request;
    const int userId = payload.value(QStringLiteral("userId")).toInt();
    if (userId <= 0) {
        return buildErrorResponse(QStringLiteral("A valid userId is required"));
    }

    const auto topics = m_stageRepository->getAllTopics();
    const auto stageItems = m_stageRepository->getStageMapForUser(userId);
    if (!topics.has_value() || !stageItems.has_value()) {
        return buildErrorResponse(QStringLiteral("Could not load stage map"));
    }
    return buildResponse(stageMapToVariant(userId, *topics, *stageItems));
}

QString BridgeController::completeStage(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for completeStage"));
    }
    if (m_stageRepository == nullptr || m_recordRepository == nullptr) {
        return buildErrorResponse(QStringLiteral("Stage or record repository is unavailable"));
    }

    const int userId = req.value(QStringLiteral("userId")).toInt();
    const int stageId = req.value(QStringLiteral("stageId")).toInt();
    const int score = req.value(QStringLiteral("score")).toInt(-1);
    const int completionTime = req.value(QStringLiteral("completionTime")).toInt(-1);
    if (userId <= 0 || stageId <= 0 || score < 0 || completionTime < 0) {
        return buildErrorResponse(QStringLiteral("Invalid user, stage, score, or completion time"));
    }

    const std::optional<Stage> stage = m_stageRepository->getStageById(stageId);
    if (!stage.has_value()) {
        return buildErrorResponse(QStringLiteral("Stage was not found"));
    }

    const auto unlockedStages = m_stageRepository->getStageMapForUser(userId);
    if (!unlockedStages.has_value()) {
        return buildErrorResponse(QStringLiteral("Could not load stage map for user"));
    }
    const auto completedStage =
            std::find_if(unlockedStages->begin(), unlockedStages->end(), [stageId](const StageMapItem &item) {
                return item.stage_id == stageId;
            });
    if (completedStage == unlockedStages->end()) {
        return buildErrorResponse(QStringLiteral("Stage was not found"));
    }
    if (!completedStage->is_unlocked) {
        return buildErrorResponse(QStringLiteral("Stage is locked and cannot be completed"));
    }

    const std::optional<StageRecord> insertedRecord =
            m_recordRepository->insertRecord(userId, stageId, score, completionTime);
    if (!insertedRecord.has_value()) {
        return buildErrorResponse(QStringLiteral("Could not save the stage record"));
    }
    const std::optional<StageRecord> bestRecord = m_recordRepository->getBestRecord(userId, stageId);
    if (!bestRecord.has_value()) {
        return buildErrorResponse(QStringLiteral("Could not load the saved stage record"));
    }

    int nextStageId = 0;
    bool nextStageUnlocked = false;
    const auto stages = m_stageRepository->getStagesWithUnlockStatus(userId);
    if (!stages.has_value()) {
        return buildErrorResponse(QStringLiteral("Record saved, but stage progress could not be loaded"));
    }
    const auto topics = m_stageRepository->getAllTopics();
    const auto stageMap = m_stageRepository->getStageMapForUser(userId);
    if (!topics.has_value() || !stageMap.has_value()) {
        return buildErrorResponse(QStringLiteral("Record saved, but the stage map could not be refreshed"));
    }
    const std::vector<CEFRLevel> cefrLevels = {CEFRLevel::A1, CEFRLevel::A2, CEFRLevel::B1,
                                               CEFRLevel::B2, CEFRLevel::C1, CEFRLevel::C2};
    const std::optional<CEFRLevel> currentLevel = cefrLevelFromName(stage->cefr_level);
    if (currentLevel.has_value()) {
        const auto currentLevelPosition = std::find(cefrLevels.begin(), cefrLevels.end(), *currentLevel);
        if (currentLevelPosition != cefrLevels.end() && currentLevelPosition + 1 != cefrLevels.end()) {
            const std::string nextLevel = cefrLevelName(*(currentLevelPosition + 1));
            const auto nextStage =
                    std::find_if(stages->begin(), stages->end(), [&stage, &nextLevel](const auto &status) {
                        return status.stage.topic_id == stage->topic_id && status.stage.cefr_level == nextLevel;
                    });
            if (nextStage != stages->end()) {
                nextStageId = nextStage->stage.stage_id;
                nextStageUnlocked = nextStage->is_unlocked;
            }
        }
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("recordId")] = insertedRecord->record_id;
    response[QStringLiteral("highScore")] = bestRecord->score;
    response[QStringLiteral("bestTime")] = bestRecord->completion_time;
    response[QStringLiteral("nextStageUnlocked")] = nextStageUnlocked;
    response[QStringLiteral("nextStageId")] = nextStageId;
    response[QStringLiteral("stageMap")] = stageMapToVariant(userId, *topics, *stageMap);

    emit stageCompleted(stageId, score);
    emit stageMapChanged(userId);
    return buildResponse(response);
}

QString BridgeController::processAnswer(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for processAnswer"));
    }

    QJsonObject payload = req;
    const bool hasNestedPayload =
            req.contains(QStringLiteral("payload")) && req.value(QStringLiteral("payload")).isObject();
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

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("correct")] = isCorrect;
    response[QStringLiteral("score")] = isCorrect ? 100 : 0;
    response[QStringLiteral("timeRemaining")] = payload.value(QStringLiteral("timeRemaining")).toDouble(60.0);
    return buildResponse(response);
}

QString BridgeController::getAllTopics() {
    if (m_stageRepository == nullptr) {
        return buildErrorResponse(QStringLiteral("Stage repository is unavailable"));
    }

    const auto allTopics = m_stageRepository->getAllTopics();
    if (!allTopics.has_value()) {
        return buildErrorResponse(QStringLiteral("Could not load topics"));
    }
    QVariantList topics;
    for (const Topic &topic: *allTopics) {
        QVariantMap item;
        item[QStringLiteral("topicId")] = topic.id;
        item[QStringLiteral("topicName")] = QString::fromStdString(topic.name);
        topics.append(item);
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("topics")] = topics;
    return buildResponse(response);
}

QString BridgeController::getProfiles(const QString &) {
    if (!m_userRepository || !m_stageRepository || !m_recordRepository) {
        return buildErrorResponse(QStringLiteral("Repositories are unavailable"));
    }
    const auto users = m_userRepository->getAllUsers();
    const auto topics = m_stageRepository->getAllTopics();
    if (!users || !topics) {
        return buildErrorResponse(QStringLiteral("Could not load profiles or topics"));
    }
    std::map<int, QString> topicNames;
    for (const Topic &topic: *topics) {
        topicNames.emplace(topic.id, QString::fromStdString(topic.name));
    }

    QVariantList profiles;
    for (const User &user: *users) {
        const auto stages = m_stageRepository->getStagesWithUnlockStatus(user.user_id);
        const auto bestRecords = m_recordRepository->getBestRecords(user.user_id);
        if (!stages || !bestRecords) {
            return buildErrorResponse(QStringLiteral("Could not load profile progress"));
        }
        std::map<int, StageRecord> bestByStage;
        qint64 totalScore = 0;
        for (const StageRecord &record: *bestRecords) {
            bestByStage.emplace(record.stage_id, record);
            totalScore += record.score;
        }
        int solved = 0;
        QVariantList stageList;
        for (const StageStatusDTO &status: *stages) {
            const Stage &stage = status.stage;
            const auto topic = topicNames.find(stage.topic_id);
            if (topic == topicNames.end()) {
                return buildErrorResponse(QStringLiteral("A stage references a missing topic"));
            }
            const auto best = bestByStage.find(stage.stage_id);
            const bool passed = best != bestByStage.end() && best->second.score >= stage.pass_score;
            if (passed) {
                ++solved;
            }
            QVariantMap item;
            item[QStringLiteral("id")] = stage.stage_id;
            item[QStringLiteral("stageId")] = QStringLiteral("STAGE %1").arg(stage.stage_id, 2, 10, QLatin1Char('0'));
            item[QStringLiteral("topicId")] = stage.topic_id;
            item[QStringLiteral("previewNextStageId")] = status.preview_next_stage_id;
            item[QStringLiteral("topic")] = topic->second;
            item[QStringLiteral("level")] = QString::fromStdString(stage.cefr_level);
            item[QStringLiteral("score")] = best == bestByStage.end() ? 0 : best->second.score;
            item[QStringLiteral("status")] = passed               ? QStringLiteral("solved")
                                             : status.is_unlocked ? QStringLiteral("available")
                                                                  : QStringLiteral("locked");
            stageList.append(item);
        }
        QVariantMap profile = userToVariant(user);
        profile[QStringLiteral("score")] = totalScore;
        profile[QStringLiteral("progress")] = QStringLiteral("%1/%2").arg(solved).arg(stageList.size());
        profile[QStringLiteral("stages")] = stageList;
        profiles.append(profile);
    }
    return buildResponse({{QStringLiteral("status"), QStringLiteral("ok")}, {QStringLiteral("profiles"), profiles}});
}

QString BridgeController::createProfile(const QString &requestJson) {
    if (!m_userRepository) {
        return buildErrorResponse(QStringLiteral("User repository is unavailable"));
    }
    const QString name = parseJson(requestJson).value(QStringLiteral("name")).toString();
    static const QRegularExpression usernamePattern(QStringLiteral("^[a-zA-Z0-9_]{3,20}$"));
    if (!usernamePattern.match(name).hasMatch()) {
        return buildErrorResponse(QStringLiteral("Username must contain 3-20 letters, numbers, or underscores"));
    }
    const auto user = m_userRepository->createUser(name.toStdString());
    if (!user) {
        return buildErrorResponse(QStringLiteral("Could not create profile. The username may already exist."));
    }
    return buildResponse(
            {{QStringLiteral("status"), QStringLiteral("ok")}, {QStringLiteral("user"), userToVariant(*user)}});
}

QString BridgeController::updateAudio(const QString &requestJson) {
    if (!m_userRepository) {
        return buildErrorResponse(QStringLiteral("User repository is unavailable"));
    }
    const QJsonObject req = parseJson(requestJson);
    const int userId = req.value(QStringLiteral("userId")).toInt();
    const auto musicEnabled = req.value(QStringLiteral("musicEnabled"));
    const auto soundEnabled = req.value(QStringLiteral("soundEnabled"));
    const int musicVolume = req.value(QStringLiteral("musicVolume")).toInt(-1);
    const int soundVolume = req.value(QStringLiteral("soundVolume")).toInt(-1);
    if (userId <= 0 || !musicEnabled.isBool() || !soundEnabled.isBool() || musicVolume < 0 || musicVolume > 100 ||
        soundVolume < 0 || soundVolume > 100) {
        return buildErrorResponse(QStringLiteral("Invalid audio settings"));
    }
    if (!m_userRepository->updateAudioSettings(userId, musicEnabled.toBool(), soundEnabled.toBool(), musicVolume,
                                               soundVolume)) {
        return buildErrorResponse(QStringLiteral("Could not save audio settings"));
    }
    return buildResponse({{QStringLiteral("status"), QStringLiteral("ok")}});
}

QString BridgeController::getLeaderboard(const QString &requestJson) {
    if (!m_recordRepository) {
        return buildErrorResponse(QStringLiteral("Record repository is unavailable"));
    }
    const QJsonObject req = parseJson(requestJson);
    if (!req.contains(QStringLiteral("stageId")) ||
        (!req.value(QStringLiteral("stageId")).isNull() && req.value(QStringLiteral("stageId")).toInt() <= 0)) {
        return buildErrorResponse(QStringLiteral("A valid stageId or null is required"));
    }
    QVariantList rows;
    constexpr int limit = 100;
    if (req.value(QStringLiteral("stageId")).isNull()) {
        const auto ranks = m_recordRepository->getOverallLeaderboard(limit);
        if (!ranks) {
            return buildErrorResponse(QStringLiteral("Could not load overall leaderboard"));
        }
        for (const OverallRankDTO &rank: *ranks) {
            rows.append(QVariantMap{{QStringLiteral("rank"), rank.rank},
                                    {QStringLiteral("userId"), rank.user_id},
                                    {QStringLiteral("player"), QString::fromStdString(rank.username)},
                                    {QStringLiteral("score"), static_cast<qint64>(rank.total_score)},
                                    {QStringLiteral("completionTime"), QVariant()}});
        }
    } else {
        const auto records =
                m_recordRepository->getStageLeaderboard(req.value(QStringLiteral("stageId")).toInt(), limit);
        if (!records) {
            return buildErrorResponse(QStringLiteral("Could not load stage leaderboard"));
        }
        for (const StageRecordDTO &row: *records) {
            rows.append(QVariantMap{{QStringLiteral("id"), row.record.record_id},
                                    {QStringLiteral("rank"), row.rank},
                                    {QStringLiteral("userId"), row.record.user_id},
                                    {QStringLiteral("player"), QString::fromStdString(row.username)},
                                    {QStringLiteral("score"), row.record.score},
                                    {QStringLiteral("completionTime"), row.record.completion_time}});
        }
    }
    return buildResponse({{QStringLiteral("status"), QStringLiteral("ok")}, {QStringLiteral("records"), rows}});
}

QString BridgeController::getStagesByTopicId(int topicId) {
    if (m_stageRepository == nullptr) {
        return buildErrorResponse(QStringLiteral("Stage repository is unavailable"));
    }
    if (topicId <= 0) {
        return buildErrorResponse(QStringLiteral("A valid topicId is required"));
    }

    QVariantList stages;
    for (const Stage &stage: m_stageRepository->getStagesByTopicId(topicId)) {
        QVariantMap item;
        item[QStringLiteral("stageId")] = stage.stage_id;
        item[QStringLiteral("topicId")] = stage.topic_id;
        item[QStringLiteral("cefrLevel")] = QString::fromStdString(stage.cefr_level);
        item[QStringLiteral("timeLimit")] = stage.time_limit;
        item[QStringLiteral("questionCount")] = stage.question_count;
        item[QStringLiteral("passScore")] = stage.pass_score;
        stages.append(item);
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stages")] = stages;
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
    if (action == QStringLiteral("selectStage")) {
        return startStage(requestJson);
    }
    if (action == QStringLiteral("completeStage") || action == QStringLiteral("endGame")) {
        return completeStage(requestJson);
    }
    if (action == QStringLiteral("getStageMap")) {
        return getStageMap(requestJson);
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

    const int userId = req.value(QStringLiteral("userId")).toInt();
    const int stagesUnlocked = req.value(QStringLiteral("stagesUnlocked")).toInt();
    if (userId <= 0 || stagesUnlocked <= 0) {
        return buildErrorResponse(QStringLiteral("Invalid userId or stagesUnlocked value"));
    }
    m_unlockedStagesByUser.insert(userId, std::max(m_unlockedStagesByUser.value(userId, 1), stagesUnlocked));

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("message")] = QStringLiteral("Progress saved successfully");
    return buildResponse(response);
}

QString BridgeController::loadProgress(const QString &requestJson) {
    const QJsonObject req = parseJson(requestJson);
    if (req.isEmpty()) {
        return buildErrorResponse(QStringLiteral("Invalid JSON payload for loadProgress"));
    }
    const int userId = req.value(QStringLiteral("userId")).toInt();
    if (userId <= 0) {
        return buildErrorResponse(QStringLiteral("A valid userId is required"));
    }

    QVariantMap response;
    response[QStringLiteral("status")] = QStringLiteral("ok");
    response[QStringLiteral("stagesUnlocked")] = m_unlockedStagesByUser.value(userId, 1);
    return buildResponse(response);
}
