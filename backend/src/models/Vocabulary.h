#pragma once

#include <optional>
#include <string>

struct Vocabulary {
    int word_id = 0;
    int topic_id = 0;
    std::string cefr_level;
    std::string target_word;
    std::optional<std::string> hint_text;
    std::optional<std::string> image_path;
    std::string audio_path;
};
