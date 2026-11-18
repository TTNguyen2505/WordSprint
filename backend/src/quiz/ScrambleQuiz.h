#pragma once

#include "QuizBase.h"

class ScrambleQuiz : public QuizBase {
public:
    explicit ScrambleQuiz(int id, const WordItem &wordItem, int timeLimitSeconds = 30, int basePoints = 100);

    QuizType getType() const override { return QuizType::Scramble; }
    QString getTypeString() const override { return QStringLiteral("Scramble"); }
    QString getFormattedQuestion() const override;
    bool checkAnswer(const QString &answer) override;
    QString getHint() const override;

    QString scrambledWord() const { return m_scrambledWord; }

private:
    QString m_scrambledWord;

    void generateScramble();
};
