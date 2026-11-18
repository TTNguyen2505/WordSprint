#include "QuizFactory.h"
#include <QRandomGenerator>

#include "ScrambleQuiz.h"
#include "FillInBlankQuiz.h"
#include "ListeningQuiz.h"

// Concrete Creators
class ScrambleQuizCreator : public IQuizCreator {
public:
    std::unique_ptr<QuizBase> create(int id, const WordItem &word) const override {
        return std::make_unique<ScrambleQuiz>(id, word);
    }
    QuizType handledType() const override { return QuizType::Scramble; }
};

class FillInBlankQuizCreator : public IQuizCreator {
public:
    std::unique_ptr<QuizBase> create(int id, const WordItem &word) const override {
        return std::make_unique<FillInBlankQuiz>(id, word);
    }
    QuizType handledType() const override { return QuizType::FillInBlank; }
};

class ListeningQuizCreator : public IQuizCreator {
public:
    std::unique_ptr<QuizBase> create(int id, const WordItem &word) const override {
        return std::make_unique<ListeningQuiz>(id, word);
    }
    QuizType handledType() const override { return QuizType::Listening; }
};

QuizFactory::QuizFactory() {
    registerDefaultCreators();
}

QuizFactory& QuizFactory::instance() {
    static QuizFactory s_instance;
    return s_instance;
}

void QuizFactory::registerDefaultCreators() {
    registerCreator(QuizType::Scramble, std::make_unique<ScrambleQuizCreator>());
    registerCreator(QuizType::FillInBlank, std::make_unique<FillInBlankQuizCreator>());
    registerCreator(QuizType::Listening, std::make_unique<ListeningQuizCreator>());
}

void QuizFactory::registerCreator(QuizType type, std::unique_ptr<IQuizCreator> creator) {
    if (creator) {
        m_creators[type] = std::move(creator);
    }
}

std::unique_ptr<QuizBase> QuizFactory::createQuiz(int id, QuizType type, const WordItem &word) const {
    if (type == QuizType::Mixed) {
        return createRandomQuiz(id, word);
    }

    auto it = m_creators.find(type);
    if (it != m_creators.end() && it->second) {
        return it->second->create(id, word);
    }

    // Default fallback to ScrambleQuiz
    return std::make_unique<ScrambleQuiz>(id, word);
}

std::unique_ptr<QuizBase> QuizFactory::createRandomQuiz(int id, const WordItem &word) const {
    static const std::vector<QuizType> availableTypes = {
        QuizType::Scramble,
        QuizType::FillInBlank,
        QuizType::Listening
    };

    const int index = QRandomGenerator::global()->bounded(static_cast<int>(availableTypes.size()));
    return createQuiz(id, availableTypes[index], word);
}

std::vector<std::unique_ptr<QuizBase>> QuizFactory::createBatch(
    const std::vector<WordItem> &words, QuizType preferredType) const {
    std::vector<std::unique_ptr<QuizBase>> results;
    results.reserve(words.size());

    int currentId = 1;
    for (const auto &w : words) {
        if (preferredType == QuizType::Mixed) {
            results.push_back(createRandomQuiz(currentId++, w));
        } else {
            results.push_back(createQuiz(currentId++, preferredType, w));
        }
    }
    return results;
}

std::unique_ptr<QuizBase> QuizFactory::create(int id, QuizType type, const WordItem &word) {
    return instance().createQuiz(id, type, word);
}

std::unique_ptr<QuizBase> QuizFactory::createRandom(int id, const WordItem &word) {
    return instance().createRandomQuiz(id, word);
}

std::vector<std::unique_ptr<QuizBase>> QuizFactory::createStageQuizzes(
    const std::vector<WordItem> &words, QuizType preferredType) {
    return instance().createBatch(words, preferredType);
}
