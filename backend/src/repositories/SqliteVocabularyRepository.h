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
    std::vector<StageRecord> getStageRecordsByUserId(int user_id) override;
    std::optional<int> insertStageRecord(
        int user_id,
        int stage_id,
        int score,
        int completion_time) override;
    std::optional<UserProgress> getUserProgress(int user_id, int stage_id) override;
    bool saveUserProgress(const UserProgress& progress) override;
    bool unlockStage(int user_id, int stage_id) override;

private:
    DatabaseManager& m_databaseManager;
};
