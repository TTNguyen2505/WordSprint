#pragma once

#include "PuzzleQuestion.h"

#include <string>
#include <vector>

namespace ReactFrontend_DomainLayer {

    class FillBlankQuestion final : public PuzzleQuestion {
    public:
        FillBlankQuestion(WordItem wordItem, CEFRLevel level);

        std::string generatePrompt() override;
        bool checkAnswer(std::string input) override;

    private:
        std::string maskedPattern;
        std::vector<int> hiddenIndices;
        CEFRLevel cefrLevel;
    };

} // namespace ReactFrontend_DomainLayer
