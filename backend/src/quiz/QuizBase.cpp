#include "QuizBase.h"
#include <algorithm>

QuizBase::QuizBase(int id, const WordItem &wordItem, int timeLimitSeconds, int basePoints)
    : m_id(id)
    , m_wordItem(wordItem)
    , m_timeLimitSeconds(timeLimitSeconds)
    , m_basePoints(basePoints)
    , m_isSolved(false) {
}

QString QuizBase::getCorrectAnswer() const {
    return m_wordItem.word.trimmed().toUpper();
}

int QuizBase::calculateScore(int timeRemaining) const {
    if (!m_isSolved) {
        return 0;
    }
    const int clampedTime = std::max(0, std::min(timeRemaining, m_timeLimitSeconds));
    const double ratio = m_timeLimitSeconds > 0 ? static_cast<double>(clampedTime) / m_timeLimitSeconds : 0.0;
    const int timeBonus = static_cast<int>(ratio * 50.0);
    return m_basePoints + timeBonus;
}

QJsonObject QuizBase::toJson() const {
    QJsonObject obj;
    obj[QStringLiteral("id")] = m_id;
    obj[QStringLiteral("type")] = getTypeString();
    obj[QStringLiteral("question")] = getFormattedQuestion();
    obj[QStringLiteral("meaningVi")] = m_wordItem.meaningVi;
    obj[QStringLiteral("phonetic")] = m_wordItem.phonetic;
    obj[QStringLiteral("hint")] = getHint();
    obj[QStringLiteral("hintImage")] = m_wordItem.hintImage;
    obj[QStringLiteral("audioPath")] = m_wordItem.audioPath;
    obj[QStringLiteral("timeLimit")] = m_timeLimitSeconds;
    obj[QStringLiteral("basePoints")] = m_basePoints;
    obj[QStringLiteral("isSolved")] = m_isSolved;
    obj[QStringLiteral("userAnswer")] = m_userAnswer;
    return obj;
}
