#include "ListeningQuiz.h"

ListeningQuiz::ListeningQuiz(int id, const WordItem &wordItem, int timeLimitSeconds, int basePoints)
    : QuizBase(id, wordItem, timeLimitSeconds, basePoints) {
}

QString ListeningQuiz::getFormattedQuestion() const {
    if (!m_wordItem.phonetic.isEmpty()) {
        return QStringLiteral("[Phát âm: %1] - Nghe và nhập đúng từ vựng").arg(m_wordItem.phonetic);
    }
    return QStringLiteral("Nghe phát âm và nhập đúng từ vựng");
}

bool ListeningQuiz::checkAnswer(const QString &answer) {
    m_userAnswer = answer.trimmed().toUpper();
    m_isSolved = (m_userAnswer == getCorrectAnswer());
    return m_isSolved;
}

QString ListeningQuiz::getHint() const {
    return QStringLiteral("Loại từ: %1 | Định nghĩa: %2")
        .arg(m_wordItem.partOfSpeech)
        .arg(m_wordItem.definitionEn);
}
