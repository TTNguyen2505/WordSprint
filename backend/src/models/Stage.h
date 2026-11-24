#pragma once

#include <string>

struct Stage {
    int stage_id = 0;
    int topic_id = 0;
    std::string cefr_level;
    int time_limit = 60;
    int question_count = 10;
    int pass_score = 0;
};
