#include "FillInBlankQuiz.h"

FillInBlankQuiz::FillInBlankQuiz(int id, const WordItem &wordItem, int timeLimitSeconds, int basePoints)
    : QuizBase(id, wordItem, timeLimitSeconds, basePoints) {
    generateMask();
}

void FillInBlankQuiz::generateMask() {
    const QString upper = m_wordItem.word.trimmed().toUpper();
    m_blankIndices.clear();

    if (upper.length() <= 2) {
        m_maskedWord = upper;
        return;
    }

    m_maskedWord = upper;
    // Hide alternating vowels or letters (indices 1, 3, etc.)
    for (int i = 1; i < upper.length(); i += 2) {
        m_blankIndices.append(i);
        m_maskedWord[i] = QLatin1Char('_');
    }

    if (m_blankIndices.isEmpty() && upper.length() > 1) {
        m_blankIndices.append(1);
        m_maskedWord[1] = QLatin1Char('_');
    }
}

QString FillInBlankQuiz::getFormattedQuestion() const {
    QStringList parts;
    for (const QChar &ch : m_maskedWord) {
        parts.append(QString(ch));
    }
    return parts.join(QStringLiteral(" "));
}

bool FillInBlankQuiz::checkAnswer(const QString &answer) {
    const QString trimmed = answer.trimmed().toUpper();
    m_userAnswer = trimmed;

    // Check full word match
    if (trimmed == getCorrectAnswer()) {
        m_isSolved = true;
        return true;
    }

    // Check only the missing characters match
    QString expectedMissing;
    const QString target = getCorrectAnswer();
    for (int idx : m_blankIndices) {
        if (idx < target.length()) {
            expectedMissing.append(target[idx]);
        }
    }

    m_isSolved = (!expectedMissing.isEmpty() && trimmed == expectedMissing);
    return m_isSolved;
}

QString FillInBlankQuiz::getHint() const {
    if (!m_wordItem.exampleSentence.isEmpty()) {
        return QStringLiteral("Ví dụ: %1").arg(m_wordItem.exampleSentence);
    }
    return QStringLiteral("Nghĩa tiếng Việt: %1").arg(m_wordItem.meaningVi);
}
