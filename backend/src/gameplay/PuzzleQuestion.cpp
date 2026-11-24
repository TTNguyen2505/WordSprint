#include "PuzzleQuestion.h"

#include <QString>

#include <utility>

namespace ReactFrontend_DomainLayer {

    PuzzleQuestion::PuzzleQuestion(::WordItem wordItem, bool showHintImage) : wordItem(std::move(wordItem)) {
        if (showHintImage) {
            hintImage = this->wordItem.hint_image_path;
        }
    }

    void PuzzleQuestion::markSolved() { status = QuestionStatus::SOLVED; }

    const ::WordItem &PuzzleQuestion::getWordItem() const noexcept { return wordItem; }

    const std::string &PuzzleQuestion::getHintImage() const noexcept { return hintImage; }

    QuestionStatus PuzzleQuestion::getStatus() const noexcept { return status; }

    bool PuzzleQuestion::matchesTarget(std::string input) const {
        const QString answer = QString::fromUtf8(input.data(), static_cast<qsizetype>(input.size())).trimmed();
        const QString target = QString::fromUtf8(wordItem.target_word.data(),
                                                 static_cast<qsizetype>(wordItem.target_word.size()))
                                       .trimmed();
        return answer.compare(target, Qt::CaseInsensitive) == 0;
    }

} // namespace ReactFrontend_DomainLayer
