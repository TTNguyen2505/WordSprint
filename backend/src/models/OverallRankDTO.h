#pragma once

#include <cstdint>
#include <string>

struct OverallRankDTO {
    int user_id = 0;
    std::string username;
    std::int64_t total_score = 0;
    int completed_stages = 0;
    int rank = 0;
};
