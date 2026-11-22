#pragma once

#include "../models/Stage.h"
#include "../models/StageRecord.h"
#include "../models/Topic.h"
#include "../models/UserProgress.h"
#include "../models/Vocabulary.h"

#include <optional>
#include <string>
#include <vector>

class IVocabularyRepository {
public:
    virtual ~IVocabularyRepository() = default;

    virtual std::vector<Topic> getAllTopics() = 0;
    virtual std::vector<Stage> getStagesByTopicId(int topic_id) = 0;
    virtual std::optional<Stage> getStageById(int stage_id) = 0;
    virtual std::vector<Vocabulary> getVocabulariesByTopic(int topicId) = 0;
    virtual std::vector<Vocabulary> getVocabulariesByLevel(const std::string& cefrLevel) = 0;
    virtual std::vector<Vocabulary> getRandomVocabularies(
        int limit,
        int topicId = -1,
        const std::string& cefrLevel = "") = 0;
    virtual std::vector<StageRecord> getStageRecordsByUserId(int user_id) = 0;
    virtual std::optional<int> insertStageRecord(
        int user_id,
        int stage_id,
        int score,
        int completion_time) = 0;
    virtual std::optional<UserProgress> getUserProgress(int user_id, int stage_id) = 0;
    virtual bool saveUserProgress(const UserProgress& progress) = 0;
    virtual bool unlockStage(int user_id, int stage_id) = 0;
};
