#include "ScrambleQuestion.h"

#include <algorithm>
#include <random>
#include <utility>

namespace ReactFrontend_DomainLayer {
    namespace {
        std::mt19937 &randomEngine() {
            static thread_local std::mt19937 engine(std::random_device{}());
            return engine;
        }
    } // namespace

    ScrambleQuestion::ScrambleQuestion(WordItem wordItem) : PuzzleQuestion(std::move(wordItem)) {}

    std::string ScrambleQuestion::generatePrompt() {
        const std::string &target = wordItem.target_word;
        scrambledLetters.assign(target.begin(), target.end());
        std::vector<std::vector<std::size_t>> letterRuns;

        std::size_t index = 0;
        while (index < target.size()) {
            if (!isAlphabetic(target[index])) {
                ++index;
                continue;
            }

            std::vector<std::size_t> positions;
            while (index < target.size() && isAlphabetic(target[index])) {
                positions.push_back(index);
                ++index;
            }

            std::vector<char> letters;
            letters.reserve(positions.size());
            for (const std::size_t position: positions) {
                letters.push_back(target[position]);
            }
            std::shuffle(letters.begin(), letters.end(), randomEngine());
            for (std::size_t letterIndex = 0; letterIndex < positions.size(); ++letterIndex) {
                scrambledLetters[positions[letterIndex]] = letters[letterIndex];
            }
            letterRuns.push_back(std::move(positions));
        }

        if (scrambledLetters == std::vector<char>(target.begin(), target.end())) {
            for (const auto &positions: letterRuns) {
                bool swapped = false;
                for (std::size_t left = 0; left < positions.size() && !swapped; ++left) {
                    for (std::size_t right = left + 1; right < positions.size(); ++right) {
                        if (target[positions[left]] != target[positions[right]]) {
                            std::swap(scrambledLetters[positions[left]], scrambledLetters[positions[right]]);
                            swapped = true;
                            break;
                        }
                    }
                }
                if (swapped) {
                    break;
                }
            }
        }

        return std::string(scrambledLetters.begin(), scrambledLetters.end());
    }

    bool ScrambleQuestion::checkAnswer(std::string input) { return matchesAnswer(std::move(input)); }

} // namespace ReactFrontend_DomainLayer
