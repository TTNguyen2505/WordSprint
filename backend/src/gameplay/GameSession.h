#pragma once

#include "PuzzleQuestion.h"

#include "../models/Stage.h"

#include <memory>
#include <vector>

namespace ReactFrontend_DomainLayer {

    class GameSession final {
    public:
        void startSession(::Stage stage, std::vector<::WordItem> words);

        const std::vector<std::unique_ptr<PuzzleQuestion>> &getQuestions() const noexcept;

    private:
        ::Stage stage;
        std::vector<std::unique_ptr<PuzzleQuestion>> questions;
        int currentIndex = 0;
        int score = 0;
        int currentStreak = 0;
        int timeRemaining = 0;
        bool isPaused = false;
    };

} // namespace ReactFrontend_DomainLayer
