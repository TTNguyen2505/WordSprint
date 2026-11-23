#pragma once

#include "../models/LearningProgress.h"
#include "../models/Topic.h"
#include "../models/Vocabulary.h"

#include <optional>
#include <string>
#include <vector>

class IVocabularyRepository {
public:
    virtual ~IVocabularyRepository() = default;

    virtual std::vector<Topic> getAllTopics() = 0;
    virtual std::vector<Vocabulary> getVocabulariesByTopic(int topicId) = 0;
    virtual std::vector<Vocabulary> getVocabulariesByLevel(const std::string& cefrLevel) = 0;
    virtual std::vector<Vocabulary> getRandomVocabularies(
        int limit,
        int topicId = -1,
        const std::string& cefrLevel = "") = 0;
    virtual std::optional<LearningProgress> getProgressByVocabId(int vocabId) = 0;
    virtual bool updateLearningProgress(int vocabId, bool isCorrect) = 0;
};
