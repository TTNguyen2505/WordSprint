#pragma once

#include "IHintStrategy.h"

class RevealLetterHintStrategy : public IHintStrategy {
public:
    HintType getType() const override { return HintType::RevealLetter; }
    QString getName() const override { return QStringLiteral("Mở một chữ cái"); }
    QString getDescription() const override { return QStringLiteral("Mở ngẫu nhiên hoặc tuần tự một ký tự chưa được giải trong từ."); }
    int getCost() const override { return 20; }

    HintResult execute(QuizBase *quiz, GameContext &context) override;
};
