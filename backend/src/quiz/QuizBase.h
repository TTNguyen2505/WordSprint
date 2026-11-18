#pragma once

#include <QJsonObject>
#include <QString>
#include <memory>
#include "../model/WordItem.h"
#include "QuizTypes.h"

class QuizBase {
public:
    explicit QuizBase(int id, const WordItem &wordItem, int timeLimitSeconds = 30, int basePoints = 100);
    virtual ~QuizBase() = default;

    int id() const { return m_id; }
    const WordItem& wordItem() const { return m_wordItem; }
    int timeLimitSeconds() const { return m_timeLimitSeconds; }
    int basePoints() const { return m_basePoints; }
    bool isSolved() const { return m_isSolved; }
    QString userAnswer() const { return m_userAnswer; }

    // Pure virtual methods - Polymorphic interface
    virtual QuizType getType() const = 0;
    virtual QString getTypeString() const = 0;
    virtual QString getFormattedQuestion() const = 0;
    virtual bool checkAnswer(const QString &answer) = 0;
    virtual QString getHint() const = 0;

    // Virtual methods with standard default implementations
    virtual QString getCorrectAnswer() const;
    virtual int calculateScore(int timeRemaining) const;
    virtual QJsonObject toJson() const;

protected:
    int m_id;
    WordItem m_wordItem;
    int m_timeLimitSeconds;
    int m_basePoints;
    bool m_isSolved{false};
    QString m_userAnswer;
};
