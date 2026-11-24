#pragma once

#include <optional>
#include <string>

struct WordItem {
    int word_id = 0;
    int topic_id = 0;
    std::string cefr_level;
    std::string target_word;
    std::string hint_image_path;
    std::optional<std::string> audio_path;
};
