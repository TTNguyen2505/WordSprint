#pragma once

#include "../model/IVocabularyRepository.h"
#include <QObject>
#include <QTimer>

class GameplayViewModel : public QObject {
    Q_OBJECT
public:
    enum class GameState { Idle, Starting, Playing, Paused, Evaluating, Finished };
    Q_ENUM(GameState)

    explicit GameplayViewModel(IVocabularyRepository* repo, QObject* parent = nullptr);
    ~GameplayViewModel() override = default;

    // Properties
    Q_PROPERTY(GameState gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(int remainingTime READ remainingTime NOTIFY remainingTimeChanged)
    Q_PROPERTY(int comboStreak READ comboStreak NOTIFY comboStreakChanged)

    GameState gameState() const { return m_state; }
    int score() const { return m_score; }
    int remainingTime() const { return m_remainingTime; }
    int comboStreak() const { return m_comboStreak; }

public slots:
    void startGame(const QString& topic = QString(), int level = 0);
    void submitAnswer(const QString& answer);
    void pauseGame();
    void resumeGame();
    void restartGame();

signals:
    void gameStateChanged(GameState newState);
    void scoreChanged(int newScore);
    void remainingTimeChanged(int newRemaining);
    void comboStreakChanged(int newCombo);
    void puzzleChanged(const QString& question);
    void gameFinished();

private:
    void updateTimer();
    void nextPuzzle();
    void evaluateAnswer(bool correct);

    IVocabularyRepository* m_repo;
    QVector<PuzzleQuestion> m_puzzles;
    int m_currentIndex = -1;
    QTimer* m_timer = nullptr;
    GameState m_state = GameState::Idle;
    int m_score = 0;
    int m_remainingTime = 0; // seconds for current puzzle
    int m_comboStreak = 0;
};
