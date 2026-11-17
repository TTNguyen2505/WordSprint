// WordViewModel.h – MVVM bridge for WordList and game state
#pragma once

#include <QObject>
#include <QStringList>
#include <QVector>
#include "Word.h"
#include "WordList.h"

class WordViewModel : public QObject {
    Q_OBJECT
    // Expose the list of words as a QStringList for QML ListView
    Q_PROPERTY(QStringList words READ words NOTIFY wordsChanged)
    // Current score
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    // Result flag (true = win, false = lose)
    Q_PROPERTY(bool won READ won NOTIFY resultChanged)

public:
    explicit WordViewModel(QObject *parent = nullptr)
        : QObject(parent), m_score(0), m_won(false) {
        // Initialize with some mock words
        resetMockData();
    }

    // ----- Accessors -------------------------------------------------
    QStringList words() const {
        QStringList list;
        for (const auto &w : m_list.data())
            list << QString::fromStdString(w.text());
        return list;
    }

    int score() const { return m_score; }
    bool won() const { return m_won; }

    // ----- Model manipulation ----------------------------------------
    Q_INVOKABLE void addWord(const QString &txt) {
        m_list.add(Word(txt.toStdString()));
        emit wordsChanged();
    }

    Q_INVOKABLE bool removeWord(int index) {
        bool ok = m_list.removeAt(static_cast<size_t>(index));
        if (ok) emit wordsChanged();
        return ok;
    }

    Q_INVOKABLE bool updateWord(int index, const QString &txt) {
        bool ok = m_list.updateAt(static_cast<size_t>(index), Word(txt.toStdString()));
        if (ok) emit wordsChanged();
        return ok;
    }

    Q_INVOKABLE void sortByLength() {
        m_list.sortByLength();
        emit wordsChanged();
    }

    Q_INVOKABLE void resetData(const QStringList &newWords) {
        std::vector<Word> vec;
        for (const auto &s : newWords) vec.emplace_back(s.toStdString());
        m_list.reset(vec);
        emit wordsChanged();
    }

    // ----- Game state ------------------------------------------------
    Q_INVOKABLE void finishGame(int finalScore, bool isWon) {
        m_score = finalScore;
        m_won = isWon;
        emit scoreChanged();
        emit resultChanged();
    }

    Q_INVOKABLE void resetGame() {
        m_score = 0;
        m_won = false;
        emit scoreChanged();
        emit resultChanged();
        resetMockData();
        emit wordsChanged();
    }

signals:
    void wordsChanged();
    void scoreChanged();
    void resultChanged();

private:
    void resetMockData() {
        m_list.reset({ Word("apple"), Word("banana"), Word("cherry") });
    }

    WordList m_list;
    int m_score;
    bool m_won;
};
