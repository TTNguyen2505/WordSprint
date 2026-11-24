#include "ScrambleQuestion.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <functional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ReactFrontend_DomainLayer {
    namespace {
        bool isLetter(char character) {
            return std::isalpha(static_cast<unsigned char>(character)) != 0;
        }

        std::vector<std::vector<std::size_t>> letterPositionsByWord(const std::string &target) {
            std::vector<std::vector<std::size_t>> result;
            std::vector<std::size_t> currentWord;
            for (std::size_t index = 0; index < target.size(); ++index) {
                if (target[index] == ' ') {
                    if (!currentWord.empty()) {
                        result.push_back(std::move(currentWord));
                        currentWord.clear();
                    }
                    continue;
                }
                if (isLetter(target[index])) {
                    currentWord.push_back(index);
                }
            }
            if (!currentWord.empty()) {
                result.push_back(std::move(currentWord));
            }
            return result;
        }

        std::mt19937 makeRandomEngine() {
            std::random_device randomDevice;
            std::seed_seq seed{randomDevice(), randomDevice(), randomDevice(), randomDevice()};
            return std::mt19937(seed);
        }

        bool writeDifferentRotation(const std::string &target,
                                    const std::vector<std::vector<std::size_t>> &positions,
                                    std::string &candidate) {
            for (const auto &wordPositions: positions) {
                std::vector<char> letters;
                letters.reserve(wordPositions.size());
                for (const std::size_t position: wordPositions) {
                    letters.push_back(target[position]);
                }
                if (std::adjacent_find(letters.begin(), letters.end(), std::not_equal_to<>()) == letters.end()) {
                    std::sort(letters.begin(), letters.end());
                    if (std::adjacent_find(letters.begin(), letters.end(), std::not_equal_to<>()) == letters.end()) {
                        continue;
                    }
                }

                for (std::size_t shift = 1; shift < letters.size(); ++shift) {
                    std::vector<char> rotated = letters;
                    std::rotate(rotated.begin(), rotated.begin() + static_cast<std::ptrdiff_t>(shift), rotated.end());
                    if (rotated == letters) {
                        continue;
                    }
                    candidate = target;
                    for (std::size_t index = 0; index < wordPositions.size(); ++index) {
                        candidate[wordPositions[index]] = rotated[index];
                    }
                    return candidate != target;
                }
            }
            return false;
        }
    } // namespace

    ScrambleQuestion::ScrambleQuestion(::WordItem wordItem) : PuzzleQuestion(std::move(wordItem)) {}

    std::string ScrambleQuestion::generatePrompt() {
        const std::string &target = wordItem.target_word;
        const auto positions = letterPositionsByWord(target);
        if (positions.empty()) {
            throw std::invalid_argument("ScrambleQuestion requires at least one rearrangeable word.");
        }

        std::mt19937 randomEngine = makeRandomEngine();
        std::string candidate = target;
        for (int attempt = 0; attempt < 64; ++attempt) {
            candidate = target;
            for (const auto &wordPositions: positions) {
                std::vector<char> letters;
                letters.reserve(wordPositions.size());
                for (const std::size_t position: wordPositions) {
                    letters.push_back(target[position]);
                }
                std::shuffle(letters.begin(), letters.end(), randomEngine);
                for (std::size_t index = 0; index < wordPositions.size(); ++index) {
                    candidate[wordPositions[index]] = letters[index];
                }
            }
            if (candidate != target) {
                scrambledLetters.assign(candidate.begin(), candidate.end());
                return candidate;
            }
        }

        if (!writeDifferentRotation(target, positions, candidate)) {
            throw std::invalid_argument("ScrambleQuestion cannot change a word with no distinct letter order.");
        }
        scrambledLetters.assign(candidate.begin(), candidate.end());
        return candidate;
    }

    bool ScrambleQuestion::checkAnswer(std::string input) { return matchesTarget(std::move(input)); }

} // namespace ReactFrontend_DomainLayer
