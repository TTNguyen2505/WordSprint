#include "FillBlankQuestion.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <utility>

namespace ReactFrontend_DomainLayer {
    namespace {
        std::mt19937 &randomEngine() {
            static thread_local std::mt19937 engine(std::random_device{}());
            return engine;
        }

        double hiddenRatio(CEFRLevel level) {
            switch (level) {
                case CEFRLevel::A1:
                case CEFRLevel::A2:
                    return 0.35;
                case CEFRLevel::B1:
                case CEFRLevel::B2:
                    return 0.50;
                case CEFRLevel::C1:
                case CEFRLevel::C2:
                    return 0.65;
            }
            return 0.35;
        }

        bool keepsFirstLetterVisible(CEFRLevel level) {
            return level == CEFRLevel::A1 || level == CEFRLevel::A2;
        }
    } // namespace

    FillBlankQuestion::FillBlankQuestion(WordItem wordItem, CEFRLevel level) : PuzzleQuestion(std::move(wordItem)),
                                                                              cefrLevel(level) {}

    std::string FillBlankQuestion::generatePrompt() {
        maskedPattern = wordItem.target_word;
        hiddenIndices.clear();

        std::vector<std::size_t> letterPositions;
        for (std::size_t index = 0; index < wordItem.target_word.size(); ++index) {
            if (isAlphabetic(wordItem.target_word[index])) {
                letterPositions.push_back(index);
            }
        }

        const std::size_t letterCount = letterPositions.size();
        if (letterCount < 2) {
            return maskedPattern;
        }

        const long roundedCount = std::lround(static_cast<double>(letterCount) * hiddenRatio(cefrLevel));
        const int hiddenCount = static_cast<int>(std::clamp<long>(roundedCount, 1L,
                                                                  static_cast<long>(letterCount - 1)));

        std::vector<std::size_t> candidates = letterPositions;
        if (keepsFirstLetterVisible(cefrLevel)) {
            candidates.erase(candidates.begin());
        }
        std::shuffle(candidates.begin(), candidates.end(), randomEngine());

        const int countToHide = std::min(hiddenCount, static_cast<int>(candidates.size()));
        hiddenIndices.reserve(static_cast<std::size_t>(countToHide));
        for (int index = 0; index < countToHide; ++index) {
            const std::size_t hiddenIndex = candidates[static_cast<std::size_t>(index)];
            hiddenIndices.push_back(static_cast<int>(hiddenIndex));
            maskedPattern[hiddenIndex] = '_';
        }
        std::sort(hiddenIndices.begin(), hiddenIndices.end());
        return maskedPattern;
    }

    bool FillBlankQuestion::checkAnswer(std::string input) { return matchesAnswer(std::move(input)); }

} // namespace ReactFrontend_DomainLayer
