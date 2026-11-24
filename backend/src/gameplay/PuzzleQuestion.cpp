#include "PuzzleQuestion.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace ReactFrontend_DomainLayer {

    PuzzleQuestion::PuzzleQuestion(WordItem wordItem) : wordItem(std::move(wordItem)),
                                                        hintImage(this->wordItem.hint_image_path) {}

    void PuzzleQuestion::markSolved() { status = QuestionStatus::SOLVED; }

    const std::string &PuzzleQuestion::hintImagePath() const noexcept { return hintImage; }

    const WordItem &PuzzleQuestion::sourceWord() const noexcept { return wordItem; }

    QuestionStatus PuzzleQuestion::questionStatus() const noexcept { return status; }

    bool PuzzleQuestion::isAlphabetic(char value) const noexcept {
        return std::isalpha(static_cast<unsigned char>(value)) != 0;
    }

    bool PuzzleQuestion::matchesAnswer(std::string input) const {
        const auto isSpace = [](char value) {
            return std::isspace(static_cast<unsigned char>(value)) != 0;
        };
        const auto first = std::find_if_not(input.begin(), input.end(), isSpace);
        const auto last = std::find_if_not(input.rbegin(), input.rend(), isSpace).base();
        input = first < last ? std::string(first, last) : std::string{};

        std::string expected = wordItem.target_word;
        const auto toLower = [](unsigned char value) {
            return static_cast<char>(std::tolower(value));
        };
        std::transform(input.begin(), input.end(), input.begin(), toLower);
        std::transform(expected.begin(), expected.end(), expected.begin(), toLower);
        return !expected.empty() && input == expected;
    }

} // namespace ReactFrontend_DomainLayer
