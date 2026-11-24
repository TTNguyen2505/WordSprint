#pragma once

#include "QuestionStatus.h"

#include "../models/WordItem.h"

#include <string>

namespace ReactFrontend_DomainLayer {

    class PuzzleQuestion {
    public:
        explicit PuzzleQuestion(WordItem wordItem);
        virtual ~PuzzleQuestion() = default;

        virtual std::string generatePrompt() = 0;
        virtual bool checkAnswer(std::string input) = 0;
        void markSolved();

        const std::string &hintImagePath() const noexcept;
        const WordItem &sourceWord() const noexcept;
        QuestionStatus questionStatus() const noexcept;

    protected:
        WordItem wordItem;
        QuestionStatus status = QuestionStatus::QUEUED;
        std::string hintImage;

        bool isAlphabetic(char value) const noexcept;
        bool matchesAnswer(std::string input) const;
    };

} // namespace ReactFrontend_DomainLayer
