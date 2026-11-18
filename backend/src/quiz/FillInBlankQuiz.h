#pragma once

#include "QuizBase.h"
#include <QVector>

class FillInBlankQuiz : public QuizBase {
public:
    explicit FillInBlankQuiz(int id, const WordItem &wordItem, int timeLimitSeconds = 30, int basePoints = 100);

    QuizType getType() const override { return QuizType::FillInBlank; }
    QString getTypeString() const override { return QStringLiteral("FillInBlank"); }
    QString getFormattedQuestion() const override;
    bool checkAnswer(const QString &answer) override;
    QString getHint() const override;

    QString maskedWord() const { return m_maskedWord; }
    const QVector<int>& blankIndices() const { return m_blankIndices; }

private:
    QString m_maskedWord;
    QVector<int> m_blankIndices;

    void generateMask();
};
