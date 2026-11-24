#include "FillBlankQuestion.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ReactFrontend_DomainLayer {
    namespace {
        bool isLetter(char character) {
            return std::isalpha(static_cast<unsigned char>(character)) != 0;
        }

        std::mt19937 makeRandomEngine() {
            std::random_device randomDevice;
            std::seed_seq seed{randomDevice(), randomDevice(), randomDevice(), randomDevice()};
            return std::mt19937(seed);
        }

        double hiddenRatio(const std::string &level) {
            if (level == "A1" || level == "A2") {
                return 0.35;
            }
            if (level == "B1" || level == "B2") {
                return 0.50;
            }
            if (level == "C1" || level == "C2") {
                return 0.65;
            }
            throw std::invalid_argument("FillBlankQuestion requires a supported CEFR level.");
        }
    } // namespace

    FillBlankQuestion::FillBlankQuestion(::WordItem wordItem) : PuzzleQuestion(std::move(wordItem)) {}

    std::string FillBlankQuestion::generatePrompt() {
        std::vector<int> letterPositions;
        letterPositions.reserve(wordItem.target_word.size());
        for (std::size_t index = 0; index < wordItem.target_word.size(); ++index) {
            if (isLetter(wordItem.target_word[index])) {
                letterPositions.push_back(static_cast<int>(index));
            }
        }

        const int letterCount = static_cast<int>(letterPositions.size());
        if (letterCount < 2) {
            throw std::invalid_argument("FillBlankQuestion needs at least two letters to hide a character.");
        }

        const int hiddenCount = std::clamp(
                static_cast<int>(std::round(static_cast<double>(letterCount) * hiddenRatio(wordItem.cefr_level))),
                1, letterCount - 1);
        const bool keepFirstLetter = wordItem.cefr_level == "A1" || wordItem.cefr_level == "A2";

        std::vector<int> eligiblePositions = letterPositions;
        if (keepFirstLetter) {
            eligiblePositions.erase(eligiblePositions.begin());
        }
        std::mt19937 randomEngine = makeRandomEngine();
        std::shuffle(eligiblePositions.begin(), eligiblePositions.end(), randomEngine);
        hiddenIndices.assign(eligiblePositions.begin(), eligiblePositions.begin() + hiddenCount);
        std::sort(hiddenIndices.begin(), hiddenIndices.end());

        maskedPattern = wordItem.target_word;
        for (const int index: hiddenIndices) {
            maskedPattern[static_cast<std::size_t>(index)] = '_';
        }
        return maskedPattern;
    }

    bool FillBlankQuestion::checkAnswer(std::string input) { return matchesTarget(std::move(input)); }

} // namespace ReactFrontend_DomainLayer
