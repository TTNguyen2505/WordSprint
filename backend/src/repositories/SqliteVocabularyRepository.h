#pragma once

#include "IVocabularyRepository.h"

class DatabaseManager;

class SqliteVocabularyRepository final : public IVocabularyRepository {
public:
    explicit SqliteVocabularyRepository(DatabaseManager& databaseManager);

    std::vector<Topic> getAllTopics() override;
    std::vector<Stage> getStagesByTopicId(int topic_id) override;
    std::optional<Stage> getStageById(int stage_id) override;
    std::vector<Vocabulary> getVocabulariesByTopic(int topicId) override;
    std::vector<Vocabulary> getVocabulariesByLevel(const std::string& cefrLevel) override;
    std::vector<Vocabulary> getRandomVocabularies(
        int limit,
        int topicId = -1,
        const std::string& cefrLevel = "") override;
    std::optional<LearningProgress> getProgressByVocabId(int vocabId) override;
    bool updateLearningProgress(int vocabId, bool isCorrect) override;

private:
    DatabaseManager& m_databaseManager;
};
