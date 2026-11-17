#pragma once

#include "IVocabularyRepository.h"

/**
 * @brief Simple mock implementation providing a static list of words and
 *        generating trivial puzzles. Suitable for unit testing and early UI
 *        development.
 */
class MockVocabularyRepository : public IVocabularyRepository {
    Q_OBJECT
public:
    explicit MockVocabularyRepository(QObject* parent = nullptr) : IVocabularyRepository(parent) {}
    ~MockVocabularyRepository() override = default;

    QVector<WordItem> getAllWords() const override {
        return {
            {"abate", "to become less intense"},
            {"benevolent", "well meaning and kindly"},
            {"candid", "truthful and straightforward"},
            {"dauntless", "showing fearlessness"},
            {"elucidate", "make (something) clear"},
            {"furtive", "attempting to avoid notice or attention"},
            {"gregarious", "fond of company; sociable"},
            {"hapless", "unfortunate"},
            {"iconoclast", "a person who attacks cherished beliefs"},
            {"juxtapose", "place side by side for comparison"}
        };
    }

    QVector<PuzzleQuestion> generatePuzzles(const QString& /*topic*/, int /*level*/, int count) const override {
        QVector<PuzzleQuestion> puzzles;
        const auto words = getAllWords();
        for (int i = 0; i < count && i < words.size(); ++i) {
            puzzles.append({QString("Define: %1").arg(words[i].word), words[i].definition});
        }
        return puzzles;
    }
};
