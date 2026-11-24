#include "BridgeController.h"

#include "models/CEFRLevel.h"
#include "models/StageRecordDTO.h"
#include "models/StageStatusDTO.h"
#include "models/WordItem.h"
#include "gameplay/AudioQuestion.h"
#include "gameplay/FillBlankQuestion.h"
#include "gameplay/ScrambleQuestion.h"
#include "repositories/RecordRepository.h"
#include "repositories/StageRepository.h"
#include "repositories/UserRepository.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QUrl>

#include <algorithm>
#include <array>
#include <map>
#include <random>
#include <vector>

BridgeController::BridgeController(StageRepository *stageRepository, RecordRepository *recordRepository,
                                   UserRepository *userRepository, QObject *parent) :
    QObject(parent), m_stageRepository(stageRepository), m_recordRepository(recordRepository),
    m_userRepository(userRepository) {}

namespace {
    enum class PuzzleMode { Scramble, FillBlank, Audio };

    std::mt19937 &quizRandomEngine() {
        static thread_local std::mt19937 engine(std::random_device{}());
        return engine;
    }

    std::vector<PuzzleMode> makePuzzleModeBag(std::size_t questionCount) {
        std::vector<PuzzleMode> modeCycle = {PuzzleMode::Scramble, PuzzleMode::FillBlank, PuzzleMode::Audio};
        std::shuffle(modeCycle.begin(), modeCycle.end(), quizRandomEngine());

        std::array<std::size_t, 3> remaining = {0, 0, 0};
        const auto modeIndex = [](PuzzleMode mode) {
            switch (mode) {
                case PuzzleMode::Scramble:
                    return std::size_t{0};
                case PuzzleMode::FillBlank:
                    return std::size_t{1};
                case PuzzleMode::Audio:
                    return std::size_t{2};
            }
            return std::size_t{0};
        };
        for (std::size_t index = 0; index < questionCount; ++index) {
            ++remaining[modeIndex(modeCycle[index % modeCycle.size()])];
        }

        std::vector<PuzzleMode> modes;
        modes.reserve(questionCount);
        while (modes.size() < questionCount) {
            std::vector<PuzzleMode> candidates;
            std::size_t highestRemaining = 0;
            for (const PuzzleMode mode: modeCycle) {
                const std::size_t count = remaining[modeIndex(mode)];
                const bool wouldCreateLongRun = modes.size() >= 2 && modes.back() == mode &&
                                                modes[modes.size() - 2] == mode;
                if (count == 0 || wouldCreateLongRun) {
                    continue;
                }
                if (count > highestRemaining) {
                    candidates.clear();
                    highestRemaining = count;
                }
                if (count == highestRemaining) {
                    candidates.push_back(mode);
                }
            }

            if (candidates.empty()) {
                break;
            }
            std::uniform_int_distribution<std::size_t> chooseMode(0, candidates.size() - 1);
            const PuzzleMode nextMode = candidates[chooseMode(quizRandomEngine())];
            modes.push_back(nextMode);
            --remaining[modeIndex(nextMode)];
        }
        return modes;
    }

    QString puzzleModeName(PuzzleMode mode) {
        switch (mode) {
            case PuzzleMode::Scramble:
                return QStringLiteral("scramble");
            case PuzzleMode::FillBlank:
                return QStringLiteral("fill-blank");
            case PuzzleMode::Audio:
                return QStringLiteral("audio");
        }
        return QStringLiteral("scramble");
    }

    QString hintImageSource(const std::string &path) {
        const QString input = QString::fromStdString(path).trimmed();
        if (input.isEmpty()) {
            return {};
        }
        if (input.startsWith(QStringLiteral("qrc:/")) || input.startsWith(QStringLiteral("file:"))) {
            return input;
        }
        if (input.startsWith(QStringLiteral(":/"))) {
            return QStringLiteral("qrc") + input;
        }
        if (QDir::isAbsolutePath(input)) {
            return QUrl::fromLocalFile(QFileInfo(input).absoluteFilePath()).toString(QUrl::FullyEncoded);
        }

        const QUrl parsed(input);
        if (!parsed.scheme().isEmpty()) {
            return input;
        }
        const QString normalized = QDir::cleanPath(input);
        if (normalized.startsWith(QStringLiteral("storage/"))) {
            return QStringLiteral("qrc:/") + normalized;
        }
        return QStringLiteral("qrc:/storage/") + normalized;
    }

    QVariantMap userToVariant(const User &user) {
        return {{QStringLiteral("id"), user.user_id},
                {QStringLiteral("name"), QString::fromStdString(user.username)},
                {QStringLiteral("musicEnabled"), user.bgm_enabled},
                {QStringLiteral("soundEnabled"), user.sfx_enabled},
                {QStringLiteral("musicVolume"), user.bgm_volume},
                {QStringLiteral("soundVolume"), user.sfx_volume}};
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

    const QString trimmedRequest = requestJson.trimmed();
    int stageId = 0;
    if (trimmedRequest.startsWith(QLatin1Char('{'))) {
        const QJsonObject request = parseJson(trimmedRequest);
        stageId = request.value(QStringLiteral("stageId")).toInt();
    } else {
        bool ok = false;
        stageId = trimmedRequest.toInt(&ok);
        if (!ok) {
            stageId = 0;
        }
    }
    if (stageId <= 0) {
        return buildErrorResponse(QStringLiteral("A valid stageId is required"));
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

    QVariantList words;
    std::vector<PuzzleMode> modes = makePuzzleModeBag(wordItems.size());
    std::vector<unsigned char> audioAvailable(wordItems.size(), 0);
    for (std::size_t index = 0; index < wordItems.size(); ++index) {
        const ReactFrontend_DomainLayer::AudioQuestion audioQuestion(wordItems[index], *level);
        audioAvailable[index] = audioQuestion.isValid() ? 1 : 0;
    }

    for (std::size_t index = 0; index < wordItems.size(); ++index) {
        const WordItem &word = wordItems[index];
        PuzzleMode mode = modes[index];

        const auto wouldCreateLongRun = [&modes, index](PuzzleMode candidate) {
            return index >= 2 && modes[index - 1] == candidate && modes[index - 2] == candidate;
        };
        if ((mode == PuzzleMode::Audio && audioAvailable[index] == 0) || wouldCreateLongRun(mode)) {
            std::vector<PuzzleMode> fallbackModes = {PuzzleMode::Scramble, PuzzleMode::FillBlank};
            if (audioAvailable[index] != 0) {
                fallbackModes.push_back(PuzzleMode::Audio);
            }
            fallbackModes.erase(std::remove_if(fallbackModes.begin(), fallbackModes.end(), wouldCreateLongRun),
                                fallbackModes.end());
            if (!fallbackModes.empty()) {
                std::uniform_int_distribution<std::size_t> chooseMode(0, fallbackModes.size() - 1);
                mode = fallbackModes[chooseMode(quizRandomEngine())];
            }
        }
        modes[index] = mode;

        std::string prompt;
        std::string imagePath;
        std::string audioPath;
        int maxReplays = 0;
        int remainingReplays = 0;
        bool autoPlay = false;

        switch (mode) {
            case PuzzleMode::Scramble: {
                ReactFrontend_DomainLayer::ScrambleQuestion question(word);
                prompt = question.generatePrompt();
                imagePath = question.hintImagePath();
                break;
            }
            case PuzzleMode::FillBlank: {
                ReactFrontend_DomainLayer::FillBlankQuestion question(word, *level);
                prompt = question.generatePrompt();
                imagePath = question.hintImagePath();
                break;
            }
            case PuzzleMode::Audio: {
                ReactFrontend_DomainLayer::AudioQuestion question(word, *level);
                prompt = question.generatePrompt();
                // AudioQuestion clears the image hint and only keeps a readable audio asset.
                imagePath = question.hintImagePath();
                audioPath = question.audioPathValue();
                maxReplays = question.maxReplaysCount();
                remainingReplays = question.remainingReplays();
                autoPlay = question.autoPlay();
                break;
            }
        }

        QVariantMap item;
        item[QStringLiteral("word_id")] = word.word_id;
        item[QStringLiteral("topic_id")] = word.topic_id;
        item[QStringLiteral("cefr_level")] = QString::fromStdString(word.cefr_level);
        item[QStringLiteral("target_word")] = QString::fromStdString(word.target_word);
        item[QStringLiteral("hint_text")] = QString();
        item[QStringLiteral("mode")] = puzzleModeName(mode);
        item[QStringLiteral("prompt")] = QString::fromStdString(prompt);
        item[QStringLiteral("image_path")] = QString::fromStdString(imagePath);
        item[QStringLiteral("hint_image_path")] = QString::fromStdString(imagePath);
        item[QStringLiteral("hintImagePath")] = hintImageSource(imagePath);
        const QVariant serializedAudioPath = audioPath.empty() ? QVariant() : QVariant(QString::fromStdString(audioPath));
        item[QStringLiteral("audio_path")] = serializedAudioPath;
        item[QStringLiteral("audioPath")] = serializedAudioPath;
        item[QStringLiteral("maxReplays")] = maxReplays;
        item[QStringLiteral("remainingReplays")] = remainingReplays;
        item[QStringLiteral("autoPlay")] = autoPlay;
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

    emit stageCompleted(stageId, score);
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
