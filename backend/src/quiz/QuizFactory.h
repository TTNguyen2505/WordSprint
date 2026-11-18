#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "QuizBase.h"
#include "QuizTypes.h"
#include "../model/WordItem.h"

// Abstract Creator in Factory Method pattern
class IQuizCreator {
public:
    virtual ~IQuizCreator() = default;
    virtual std::unique_ptr<QuizBase> create(int id, const WordItem &word) const = 0;
    virtual QuizType handledType() const = 0;
};

// Factory Method implementation class
class QuizFactory {
public:
    QuizFactory();
    ~QuizFactory() = default;

    // Singleton or Instance access
    static QuizFactory& instance();

    // Factory Method: Creates a Quiz instance via unique_ptr<QuizBase>
    std::unique_ptr<QuizBase> createQuiz(int id, QuizType type, const WordItem &word) const;
    std::unique_ptr<QuizBase> createRandomQuiz(int id, const WordItem &word) const;
    std::vector<std::unique_ptr<QuizBase>> createBatch(const std::vector<WordItem> &words, QuizType preferredType = QuizType::Mixed) const;

    // Extension point: register custom quiz creator without modifying existing code
    void registerCreator(QuizType type, std::unique_ptr<IQuizCreator> creator);

    // Static helper methods
    static std::unique_ptr<QuizBase> create(int id, QuizType type, const WordItem &word);
    static std::unique_ptr<QuizBase> createRandom(int id, const WordItem &word);
    static std::vector<std::unique_ptr<QuizBase>> createStageQuizzes(const std::vector<WordItem> &words, QuizType preferredType = QuizType::Mixed);

private:
    std::unordered_map<QuizType, std::unique_ptr<IQuizCreator>> m_creators;
    void registerDefaultCreators();
};
