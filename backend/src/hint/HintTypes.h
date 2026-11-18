#pragma once

#include <QJsonObject>
#include <QSet>
#include <QString>

enum class HintType {
    RevealLetter = 1,
    FreezeTime = 2,
    ShowImage = 3,
    Custom = 99
};

struct GameContext {
    int remainingSeconds{60};
    int totalScore{100};
    int freezeBonusSeconds{0};
    bool isTimeFrozen{false};
    QSet<int> revealedIndices;

    void deductScore(int points) {
        totalScore = std::max(0, totalScore - points);
    }
};

struct HintResult {
    bool success{false};
    HintType type{HintType::RevealLetter};
    QString message;
    QJsonObject payload;
    int costDeducted{0};

    QJsonObject toJson() const {
        QJsonObject obj;
        obj[QStringLiteral("success")] = success;
        obj[QStringLiteral("type")] = static_cast<int>(type);
        obj[QStringLiteral("message")] = message;
        obj[QStringLiteral("costDeducted")] = costDeducted;
        obj[QStringLiteral("payload")] = payload;
        return obj;
    }
};
