#pragma once

#include "PuzzleQuestion.h"

#include <vector>

namespace ReactFrontend_DomainLayer {

    class FillBlankQuestion final : public PuzzleQuestion {
    public:
        explicit FillBlankQuestion(::WordItem wordItem);

        std::string generatePrompt() override;
        bool checkAnswer(std::string input) override;

    private:
        std::string maskedPattern;
        std::vector<int> hiddenIndices;
    };

} // namespace ReactFrontend_DomainLayer
