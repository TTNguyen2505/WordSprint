#pragma once

#include "QuizBase.h"

class ListeningQuiz : public QuizBase {
public:
    explicit ListeningQuiz(int id, const WordItem &wordItem, int timeLimitSeconds = 30, int basePoints = 120);

    QuizType getType() const override { return QuizType::Listening; }
    QString getTypeString() const override { return QStringLiteral("Listening"); }
    QString getFormattedQuestion() const override;
    bool checkAnswer(const QString &answer) override;
    QString getHint() const override;

    QString audioPath() const { return m_wordItem.audioPath; }
    QString phonetic() const { return m_wordItem.phonetic; }
};
