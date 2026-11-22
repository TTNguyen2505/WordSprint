#pragma once

#include <optional>

struct UserProgress {
    int user_id = 0;
    int stage_id = 0;
    int is_unlocked = 0;
    int is_completed = 0;
    int high_score = 0;
    std::optional<int> best_time;
};
