#include "HintManager.h"
#include "RevealLetterHintStrategy.h"
#include "FreezeTimeHintStrategy.h"
#include "ShowImageHintStrategy.h"

HintManager::HintManager() {
    registerDefaultStrategies();
}

HintManager& HintManager::instance() {
    static HintManager s_instance;
    return s_instance;
}

void HintManager::registerDefaultStrategies() {
    registerStrategy(std::make_unique<RevealLetterHintStrategy>());
    registerStrategy(std::make_unique<FreezeTimeHintStrategy>());
    registerStrategy(std::make_unique<ShowImageHintStrategy>());
}

void HintManager::registerStrategy(std::unique_ptr<IHintStrategy> strategy) {
    if (strategy) {
        m_strategies[strategy->getType()] = std::move(strategy);
    }
}

bool HintManager::hasStrategy(HintType type) const {
    return m_strategies.find(type) != m_strategies.end();
}

const IHintStrategy* HintManager::getStrategy(HintType type) const {
    auto it = m_strategies.find(type);
    if (it != m_strategies.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool HintManager::canAfford(HintType type, const GameContext &context) const {
    const auto *strategy = getStrategy(type);
    if (!strategy) {
        return false;
    }
    return context.totalScore >= strategy->getCost();
}

std::vector<HintType> HintManager::getAvailableStrategies() const {
    std::vector<HintType> list;
    for (const auto &pair : m_strategies) {
        list.push_back(pair.first);
    }
    return list;
}

HintResult HintManager::applyHint(HintType type, QuizBase *quiz, GameContext &context) {
    auto it = m_strategies.find(type);
    if (it == m_strategies.end() || !it->second) {
        HintResult err;
        err.success = false;
        err.type = type;
        err.message = QStringLiteral("Chiến lược trợ giúp không được hỗ trợ.");
        return err;
    }

    if (!canAfford(type, context)) {
        HintResult err;
        err.success = false;
        err.type = type;
        err.message = QStringLiteral("Không đủ điểm để sử dụng trợ giúp này (cần %1 điểm).")
                          .arg(it->second->getCost());
        return err;
    }

    return it->second->execute(quiz, context);
}
