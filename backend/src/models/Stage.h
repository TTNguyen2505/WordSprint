#pragma once

#include <string>

struct Stage {
    int stage_id = 0;
    int topic_id = 0;
    std::string cefr_level;
    int unlock_order = 0;
    int time_limit = 60;
    int question_count = 10;
};
