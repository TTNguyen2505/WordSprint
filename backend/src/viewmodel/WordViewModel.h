// WordViewModel.h - MVVM bridge backed by WordRepository (SQLite)
#pragma once

#include <QObject>
#include <QStringList>
#include <QVector>
#include "../model/Word.h"
#include "../model/WordList.h"
#include "../repository/WordRepository.h"

class WordViewModel : public QObject {
    Q_OBJECT
    // Preserve all Q_PROPERTY bindings for QML UI
    Q_PROPERTY(QStringList words READ words NOTIFY wordsChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(bool won READ won NOTIFY resultChanged)

public:
    explicit WordViewModel(QObject* parent = nullptr);
    explicit WordViewModel(WordRepository* repository, QObject* parent = nullptr);

    // ----- Accessors -------------------------------------------------
    QStringList words() const;
    int score() const { return m_score; }
    bool won() const { return m_won; }

    // ----- Model manipulation (persisted via Repository) -------------
    Q_INVOKABLE void addWord(const QString& txt);
    Q_INVOKABLE bool removeWord(int index);
    Q_INVOKABLE bool updateWord(int index, const QString& txt);
    Q_INVOKABLE void sortByLength();
    Q_INVOKABLE void resetData(const QStringList& newWords);
    Q_INVOKABLE void filterByCefrLevel(const QString& cefrLevel);
    Q_INVOKABLE void reloadWords();

    // ----- Game state ------------------------------------------------
    Q_INVOKABLE void finishGame(int finalScore, bool isWon);
    Q_INVOKABLE void resetGame();

signals:
    void wordsChanged();
    void scoreChanged();
    void resultChanged();

private:
    void loadFromRepository();

    WordRepository* m_repository = nullptr;
    WordList m_list;
    int m_score = 0;
    bool m_won = false;
};
