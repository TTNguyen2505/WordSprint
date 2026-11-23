#pragma once

#include <string>

struct LearningProgress {
    int vocabId = 0;
    int correctCount = 0;
    int wrongCount = 0;
    int masteryLevel = 0;
    std::string lastReviewed;
};
