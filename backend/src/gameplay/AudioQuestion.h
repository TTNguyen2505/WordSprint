#pragma once

#include "PuzzleQuestion.h"

namespace ReactFrontend_DomainLayer {

    class AudioQuestion final : public PuzzleQuestion {
    public:
        explicit AudioQuestion(::WordItem wordItem);

        bool playAudio();
        std::string generatePrompt() override;
        bool checkAnswer(std::string input) override;

        const std::string &getAudioPath() const noexcept;
        int getMaxReplays() const noexcept;
        int getRemainingReplays() const noexcept;

    private:
        std::string audioPath;
        int maxReplays = 0;
        int remainingReplays = 0;
    };

} // namespace ReactFrontend_DomainLayer
