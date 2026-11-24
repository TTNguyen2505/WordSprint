#include "AudioQuestion.h"

#include <stdexcept>
#include <utility>

namespace ReactFrontend_DomainLayer {
    namespace {
        int replayCountForLevel(const std::string &level) {
            if (level == "A1" || level == "A2") {
                return 3;
            }
            if (level == "B1" || level == "B2") {
                return 2;
            }
            if (level == "C1" || level == "C2") {
                return 1;
            }
            throw std::invalid_argument("AudioQuestion requires a supported CEFR level.");
        }
    } // namespace

    AudioQuestion::AudioQuestion(::WordItem wordItem) : PuzzleQuestion(wordItem, false) {
        audioPath = wordItem.audio_path.value_or(std::string{});
        maxReplays = replayCountForLevel(wordItem.cefr_level);
        remainingReplays = maxReplays;
    }

    bool AudioQuestion::playAudio() {
        if (audioPath.empty() || remainingReplays <= 0) {
            return false;
        }
        --remainingReplays;
        return true;
    }

    std::string AudioQuestion::generatePrompt() { return audioPath; }

    bool AudioQuestion::checkAnswer(std::string input) { return matchesTarget(std::move(input)); }

    const std::string &AudioQuestion::getAudioPath() const noexcept { return audioPath; }

    int AudioQuestion::getMaxReplays() const noexcept { return maxReplays; }

    int AudioQuestion::getRemainingReplays() const noexcept { return remainingReplays; }

} // namespace ReactFrontend_DomainLayer
