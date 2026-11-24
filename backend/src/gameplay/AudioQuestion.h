#pragma once

#include "PuzzleQuestion.h"

#include <string>

namespace ReactFrontend_DomainLayer {

    class AudioQuestion final : public PuzzleQuestion {
    public:
        AudioQuestion(WordItem wordItem, CEFRLevel level);

        std::string generatePrompt() override;
        bool checkAnswer(std::string input) override;
        bool playAudio();
        bool consumeReplay();

        const std::string &audioPathValue() const noexcept;
        int maxReplaysCount() const noexcept;
        int remainingReplays() const noexcept;
        bool autoPlay() const noexcept;
        bool isValid() const noexcept;

    private:
        std::string audioPath;
        int maxReplays = 0;
        int remainingReplayCount = 0;
        bool autoPlayOnLoad = false;
    };

} // namespace ReactFrontend_DomainLayer
