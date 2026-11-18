#pragma once

#include "IHintStrategy.h"

class FreezeTimeHintStrategy : public IHintStrategy {
public:
    explicit FreezeTimeHintStrategy(int freezeSeconds = 10);

    HintType getType() const override { return HintType::FreezeTime; }
    QString getName() const override { return QStringLiteral("Đóng băng thời gian"); }
    QString getDescription() const override { return QStringLiteral("Đóng băng thời gian và cộng thêm giây đếm ngược."); }
    int getCost() const override { return 15; }

    HintResult execute(QuizBase *quiz, GameContext &context) override;

private:
    int m_freezeSeconds;
};
