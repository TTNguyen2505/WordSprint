#include "ShowImageHintStrategy.h"
#include "../quiz/QuizBase.h"

HintResult ShowImageHintStrategy::execute(QuizBase *quiz, GameContext &context) {
    HintResult result;
    result.type = HintType::ShowImage;

    if (!quiz) {
        result.success = false;
        result.message = QStringLiteral("Câu đố không tồn tại.");
        return result;
    }

    const QString imagePath = quiz->wordItem().hintImage;
    if (imagePath.trimmed().isEmpty()) {
        result.success = false;
        result.message = QStringLiteral("Không có ảnh minh họa cho từ vựng này.");
        return result;
    }

    context.deductScore(getCost());

    result.success = true;
    result.costDeducted = getCost();
    result.message = QStringLiteral("Đã mở ảnh minh họa gợi ý.");

    QJsonObject payload;
    payload[QStringLiteral("imagePath")] = imagePath;
    payload[QStringLiteral("meaningVi")] = quiz->wordItem().meaningVi;
    result.payload = payload;

    return result;
}
