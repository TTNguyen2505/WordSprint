#pragma once

#include <string>

struct StageMapItem {
    int stage_id = 0;
    int topic_id = 0;
    std::string topic_name;
    std::string cefr_level;
    int question_count = 0;
    int time_limit = 0;
    int pass_score = 0;
    bool is_unlocked = false;
    bool is_passed = false;
    int best_score = 0;
    int best_time = 0;
    int attempt_count = 0;
};
