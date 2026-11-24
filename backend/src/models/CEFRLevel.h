#pragma once

#include <optional>
#include <string>

enum class CEFRLevel { A1, A2, B1, B2, C1, C2 };

inline std::string cefrLevelName(CEFRLevel level) {
    switch (level) {
        case CEFRLevel::A1:
            return "A1";
        case CEFRLevel::A2:
            return "A2";
        case CEFRLevel::B1:
            return "B1";
        case CEFRLevel::B2:
            return "B2";
        case CEFRLevel::C1:
            return "C1";
        case CEFRLevel::C2:
            return "C2";
    }
    return {};
}

inline std::optional<CEFRLevel> cefrLevelFromName(const std::string &name) {
    if (name == "A1") {
        return CEFRLevel::A1;
    }
    if (name == "A2") {
        return CEFRLevel::A2;
    }
    if (name == "B1") {
        return CEFRLevel::B1;
    }
    if (name == "B2") {
        return CEFRLevel::B2;
    }
    if (name == "C1") {
        return CEFRLevel::C1;
    }
    if (name == "C2") {
        return CEFRLevel::C2;
    }
    return std::nullopt;
}
