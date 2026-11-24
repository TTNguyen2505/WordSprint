#pragma once

#include <cstdint>
#include <string>

struct StageRecord {
    int record_id = 0;
    int user_id = 0;
    int stage_id = 0;
    int score = 0;
    int completion_time = 0;
    std::int64_t played_at = 0;
};
