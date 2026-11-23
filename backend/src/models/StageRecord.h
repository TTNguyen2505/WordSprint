#pragma once

#include <string>

struct StageRecord {
    int record_id = 0;
    int user_id = 0;
    int stage_id = 0;
    int score = 0;
    int completion_time = 0;
    std::string played_at;
};
