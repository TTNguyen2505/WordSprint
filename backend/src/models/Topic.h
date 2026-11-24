#pragma once

#include <optional>
#include <string>

struct Topic {
    int topic_id = 0;
    std::string topic_name;
    std::optional<std::string> description;
};
