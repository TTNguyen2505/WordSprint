// VocabularyRepository.h
// Concrete implementation of IVocabularyRepository using QtSql and DatabaseManager.

#pragma once

#include "IVocabularyRepository.h"
#include "DatabaseManager.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlDriver>

#include <memory>

class VocabularyRepository : public IVocabularyRepository {
public:
    // Accept a shared DatabaseManager – caller owns its lifetime (e.g., singleton).
    explicit VocabularyRepository(std::shared_ptr<DatabaseManager> dbMgr);
    ~VocabularyRepository() override = default;

    // IVocabularyRepository overrides
    std::vector<Level> getAllLevels() override;
    std::vector<Topic> getAllTopics() override;
    std::vector<Vocabulary> getVocabulariesByLevel(int levelId) override;
    std::vector<Vocabulary> getVocabulariesByTopic(int topicId) override;
    std::vector<Vocabulary> getVocabulariesFilter(const std::string &filter,
                                                 const std::vector<std::string> &params) override;
    std::optional<Vocabulary> getVocabularyById(int vocabId) override;
    std::vector<Vocabulary> searchVocabularies(const std::string &keyword) override;

private:
    std::shared_ptr<DatabaseManager> m_dbMgr;

    // Helper to map a QSqlQuery row to structs.
    Level mapToLevel(const QSqlQuery &q);
    Topic mapToTopic(const QSqlQuery &q);
    Vocabulary mapToVocabulary(const QSqlQuery &q);
};
