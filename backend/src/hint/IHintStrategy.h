#pragma once

#include <QString>
#include "HintTypes.h"

class QuizBase;

// Strategy Interface
class IHintStrategy {
public:
    virtual ~IHintStrategy() = default;

    virtual HintType getType() const = 0;
    virtual QString getName() const = 0;
    virtual QString getDescription() const = 0;
    virtual int getCost() const = 0;
    virtual HintResult execute(QuizBase *quiz, GameContext &context) = 0;
};
