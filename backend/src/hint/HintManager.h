#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "IHintStrategy.h"
#include "HintTypes.h"

class QuizBase;

// Context class in Strategy Pattern
// Manages hint strategies and adheres to Open/Closed Principle
class HintManager {
public:
    HintManager();
    ~HintManager() = default;

    static HintManager& instance();

    // Strategy registration (Open/Closed Principle extension point)
    void registerStrategy(std::unique_ptr<IHintStrategy> strategy);

    // Context execution
    HintResult applyHint(HintType type, QuizBase *quiz, GameContext &context);

    bool hasStrategy(HintType type) const;
    const IHintStrategy* getStrategy(HintType type) const;
    bool canAfford(HintType type, const GameContext &context) const;
    std::vector<HintType> getAvailableStrategies() const;

private:
    std::unordered_map<HintType, std::unique_ptr<IHintStrategy>> m_strategies;

    void registerDefaultStrategies();
};
