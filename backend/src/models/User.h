#pragma once

#include <cstdint>
#include <string>

struct User {
    int user_id = 0;
    std::string username;
    std::int64_t created_at = 0;
    bool bgm_enabled = true;
    bool sfx_enabled = true;
    int bgm_volume = 100;
    int sfx_volume = 100;
};
