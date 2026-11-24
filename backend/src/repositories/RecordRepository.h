#pragma once

#include "../models/OverallRankDTO.h"
#include "../models/StageRecord.h"
#include "../models/StageRecordDTO.h"

#include <optional>
#include <vector>

class DatabaseManager;

class RecordRepository final {
public:
    explicit RecordRepository(DatabaseManager &databaseManager);

    std::optional<StageRecord> insertRecord(int userId, int stageId, int score, int completionTime);
    std::optional<StageRecord> getBestRecord(int userId, int stageId);
    std::optional<std::vector<StageRecord>> getBestRecords(int userId);
    std::optional<std::vector<StageRecordDTO>> getStageLeaderboard(int stageId, int limit);
    std::optional<std::vector<OverallRankDTO>> getOverallLeaderboard(int limit);

private:
    DatabaseManager &m_databaseManager;
};
