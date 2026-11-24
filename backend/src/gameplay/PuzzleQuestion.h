#pragma once

#include "../models/WordItem.h"

#include <string>

namespace ReactFrontend_DomainLayer {

    enum class QuestionStatus { QUEUED, CURRENT, SOLVED };

    class GameSession;

    class PuzzleQuestion {
    public:
        explicit PuzzleQuestion(::WordItem wordItem, bool showHintImage = true);
        virtual ~PuzzleQuestion() = default;

        virtual std::string generatePrompt() = 0;
        virtual bool checkAnswer(std::string input) = 0;
        void markSolved();

        const ::WordItem &getWordItem() const noexcept;
        const std::string &getHintImage() const noexcept;
        QuestionStatus getStatus() const noexcept;

    protected:
        bool matchesTarget(std::string input) const;

        ::WordItem wordItem;
        QuestionStatus status = QuestionStatus::QUEUED;
        std::string hintImage;

    private:
        friend class GameSession;
    };

} // namespace ReactFrontend_DomainLayer
