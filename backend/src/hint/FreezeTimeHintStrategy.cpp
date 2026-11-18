#include "FreezeTimeHintStrategy.h"
#include "../quiz/QuizBase.h"

FreezeTimeHintStrategy::FreezeTimeHintStrategy(int freezeSeconds)
    : m_freezeSeconds(freezeSeconds) {
}

HintResult FreezeTimeHintStrategy::execute(QuizBase *quiz, GameContext &context) {
    Q_UNUSED(quiz);

    HintResult result;
    result.type = HintType::FreezeTime;

    context.isTimeFrozen = true;
    context.freezeBonusSeconds += m_freezeSeconds;
    context.remainingSeconds += m_freezeSeconds;
    context.deductScore(getCost());

    result.success = true;
    result.costDeducted = getCost();
    result.message = QStringLiteral("Đã kích hoạt đóng băng thời gian trong %1 giây!").arg(m_freezeSeconds);

    QJsonObject payload;
    payload[QStringLiteral("freezeSeconds")] = m_freezeSeconds;
    payload[QStringLiteral("isTimeFrozen")] = true;
    payload[QStringLiteral("remainingSeconds")] = context.remainingSeconds;
    result.payload = payload;

    return result;
}
