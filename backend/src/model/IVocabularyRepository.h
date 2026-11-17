#pragma once

#include <QObject>
#include <QString>
#include <QVector>

struct WordItem {
    QString word;
    QString definition;
};

struct PuzzleQuestion {
    QString question;
    QString answer;
};

/**
 * @brief Abstract interface for a vocabulary repository.
 *
 * The repository provides the list of words and can generate puzzle
 * questions used by the gameplay view‑model. It is deliberately tiny –
 * only the operations needed for OOP‑21 are declared.
 */
class IVocabularyRepository : public QObject {
    Q_OBJECT
public:
    explicit IVocabularyRepository(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IVocabularyRepository() = default;

    /** Return the full list of words. */
    virtual QVector<WordItem> getAllWords() const = 0;

    /**
     * Generate a set of puzzles based on a topic/level.
     * @param topic   ignored in the mock implementation.
     * @param level   ignored in the mock implementation.
     * @param count   number of puzzles to generate.
     */
    virtual QVector<PuzzleQuestion> generatePuzzles(const QString& topic,
                                                   int level,
                                                   int count) const = 0;
};
