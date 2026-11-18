#pragma once

#include "IHintStrategy.h"

class ShowImageHintStrategy : public IHintStrategy {
public:
    HintType getType() const override { return HintType::ShowImage; }
    QString getName() const override { return QStringLiteral("Xem ảnh minh họa"); }
    QString getDescription() const override { return QStringLiteral("Hiển thị hình ảnh gợi ý trực quan liên quan đến từ vựng."); }
    int getCost() const override { return 10; }

    HintResult execute(QuizBase *quiz, GameContext &context) override;
};
