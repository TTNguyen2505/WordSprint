#include "RevealLetterHintStrategy.h"
#include "../quiz/QuizBase.h"

HintResult RevealLetterHintStrategy::execute(QuizBase *quiz, GameContext &context) {
    HintResult result;
    result.type = HintType::RevealLetter;

    if (!quiz) {
        result.success = false;
        result.message = QStringLiteral("Câu đố không tồn tại.");
        return result;
    }

    const QString target = quiz->getCorrectAnswer();
    int targetIdx = -1;

    for (int i = 0; i < target.length(); ++i) {
        if (!context.revealedIndices.contains(i)) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx == -1) {
        result.success = false;
        result.message = QStringLiteral("Toàn bộ các ký tự đã được hiển thị.");
        return result;
    }

    context.revealedIndices.insert(targetIdx);
    context.deductScore(getCost());

    result.success = true;
    result.costDeducted = getCost();
    result.message = QStringLiteral("Đã mở ký tự '%1' tại vị trí %2.")
                         .arg(target[targetIdx])
                         .arg(targetIdx + 1);

    QJsonObject payload;
    payload[QStringLiteral("index")] = targetIdx;
    payload[QStringLiteral("character")] = QString(target[targetIdx]);
    payload[QStringLiteral("revealedCount")] = context.revealedIndices.size();
    result.payload = payload;

    return result;
}
