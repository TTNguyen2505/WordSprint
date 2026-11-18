// IVocabularyRepository.h
// Interface defining data-access operations required by ViewModel.
// Pure C++ signatures; implementation will use QtSql.

#pragma once

#include <vector>
#include <string>
#include <optional>

#include "Level.h"
#include "Topic.h"
#include "Vocabulary.h"

class IVocabularyRepository {
public:
    virtual ~IVocabularyRepository() = default;

    // Retrieve all levels ordered by id.
    virtual std::vector<Level> getAllLevels() = 0;

    // Retrieve all topics ordered by id.
    virtual std::vector<Topic> getAllTopics() = 0;

    // Vocabularies filtered by a specific level id.
    virtual std::vector<Vocabulary> getVocabulariesByLevel(int levelId) = 0;

    // Vocabularies filtered by a specific topic id.
    virtual std::vector<Vocabulary> getVocabulariesByTopic(int topicId) = 0;

    // Generic filter – allow caller to pass arbitrary WHERE clause fragment.
    // `filter` should be a valid SQL fragment (e.g., "level_id = ? AND word LIKE ?").
    virtual std::vector<Vocabulary> getVocabulariesFilter(const std::string &filter,
                                                         const std::vector<std::string> &params) = 0;

    // Get a single Vocabulary by primary key.
    virtual std::optional<Vocabulary> getVocabularyById(int vocabId) = 0;

    // Full‑text search across word, meaning_vi, definition_en, and example_sentence.
    // Returns rows that contain the `keyword` (case‑insensitive).
    virtual std::vector<Vocabulary> searchVocabularies(const std::string &keyword) = 0;
};
