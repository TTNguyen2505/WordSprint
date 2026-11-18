#include "ScrambleQuiz.h"
#include <QRandomGenerator>

ScrambleQuiz::ScrambleQuiz(int id, const WordItem &wordItem, int timeLimitSeconds, int basePoints)
    : QuizBase(id, wordItem, timeLimitSeconds, basePoints) {
    generateScramble();
}

void ScrambleQuiz::generateScramble() {
    QString upper = m_wordItem.word.trimmed().toUpper();
    if (upper.length() <= 1) {
        m_scrambledWord = upper;
        return;
    }

    m_scrambledWord = upper;
    int attempts = 0;
    while (attempts < 10 && m_scrambledWord == upper) {
        for (int i = m_scrambledWord.length() - 1; i > 0; --i) {
            int j = QRandomGenerator::global()->bounded(i + 1);
            std::swap(m_scrambledWord[i], m_scrambledWord[j]);
        }
        attempts++;
    }

    if (m_scrambledWord == upper && upper.length() >= 2) {
        std::swap(m_scrambledWord[0], m_scrambledWord[1]);
    }
}

QString ScrambleQuiz::getFormattedQuestion() const {
    QStringList letters;
    for (const QChar &ch : m_scrambledWord) {
        letters.append(QString(ch));
    }
    return letters.join(QStringLiteral("  "));
}

bool ScrambleQuiz::checkAnswer(const QString &answer) {
    m_userAnswer = answer.trimmed().toUpper();
    m_isSolved = (m_userAnswer == getCorrectAnswer());
    return m_isSolved;
}

QString ScrambleQuiz::getHint() const {
    const QString target = getCorrectAnswer();
    if (target.length() <= 2) {
        return QStringLiteral("Nghĩa: %1").arg(m_wordItem.meaningVi);
    }
    return QStringLiteral("Bắt đầu bằng '%1', kết thúc bằng '%2'. Nghĩa: %3")
        .arg(target.front())
        .arg(target.back())
        .arg(m_wordItem.meaningVi);
}
