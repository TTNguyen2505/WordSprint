#pragma once

#include "../models/CEFRLevel.h"
#include "../models/Stage.h"
#include "../models/StageMapItem.h"
#include "../models/StageStatusDTO.h"
#include "../models/Topic.h"
#include "../models/WordItem.h"

#include <optional>
#include <vector>

class DatabaseManager;

class StageRepository final {
public:
    explicit StageRepository(DatabaseManager &databaseManager);

    std::optional<std::vector<Topic>> getAllTopics();
    std::vector<Stage> getStagesByTopicId(int topicId);
    std::optional<Stage> getStageById(int stageId);
    std::optional<std::vector<StageStatusDTO>> getStagesWithUnlockStatus(int userId);
    std::optional<std::vector<StageMapItem>> getStageMapForUser(int userId);
    std::vector<WordItem> getRandomWords(int topicId, CEFRLevel level, int limit);

private:
    DatabaseManager &m_databaseManager;
};
