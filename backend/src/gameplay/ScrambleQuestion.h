#pragma once

#include "PuzzleQuestion.h"

#include <vector>

namespace ReactFrontend_DomainLayer {

    class ScrambleQuestion final : public PuzzleQuestion {
    public:
        explicit ScrambleQuestion(::WordItem wordItem);

        std::string generatePrompt() override;
        bool checkAnswer(std::string input) override;

    private:
        std::vector<char> scrambledLetters;
    };

} // namespace ReactFrontend_DomainLayer
