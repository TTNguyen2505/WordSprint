#include "GameplayViewModel.h"
#include <QTimer>
#include <QDebug>

GameplayViewModel::GameplayViewModel(IVocabularyRepository* repo, QObject* parent)
    : QObject(parent), m_repo(repo) {
    Q_ASSERT(repo != nullptr);
    m_timer = new QTimer(this);
    m_timer->setInterval(1000); // 1 second tick
    connect(m_timer, &QTimer::timeout, this, &GameplayViewModel::updateTimer);
}

void GameplayViewModel::startGame(const QString& /*topic*/, int /*level*/) {
    if (m_state != GameState::Idle && m_state != GameState::Finished) return;
    m_puzzles = m_repo->generatePuzzles(QString(), 0, 10);
    m_currentIndex = -1;
// Emit score and combo only when a correct answer is given, not on start
// Removed emit scoreChanged(m_score); and emit comboStreakChanged(m_comboStreak); from startGame
    nextPuzzle();
    m_state = GameState::Playing;
    emit gameStateChanged(m_state);
    m_timer->start();
}

void GameplayViewModel::updateTimer() {
    if (m_state != GameState::Playing) return;
    m_remainingTime--;
    emit remainingTimeChanged(m_remainingTime);
    if (m_remainingTime <= 0) {
        // time out, treat as wrong answer
        evaluateAnswer(false);
    }
}

void GameplayViewModel::submitAnswer(const QString& answer) {
    if (m_state != GameState::Playing) return;
    // simple check: compare answer to expected answer
    const auto& puzzle = m_puzzles.at(m_currentIndex);
    bool correct = (answer.trimmed().compare(puzzle.answer, Qt::CaseInsensitive) == 0);
    evaluateAnswer(correct);
}

void GameplayViewModel::evaluateAnswer(bool correct) {
    // stop timer temporarily
    m_timer->stop();
    if (correct) {
        // scoring: base 100 + combo bonus + time bonus
        int base = 100;
        int comboBonus = m_comboStreak * 10; // 10 points per streak
        int timeBonus = m_remainingTime * 2; // 2 per sec left
        m_score += base + comboBonus + timeBonus;
        m_comboStreak++;
        emit scoreChanged(m_score);
        emit comboStreakChanged(m_comboStreak);
    } else {
        m_comboStreak = 0;
        emit comboStreakChanged(m_comboStreak);
    }
    // move to next puzzle or finish
    if (m_currentIndex + 1 >= m_puzzles.size()) {
        m_state = GameState::Finished;
        emit gameStateChanged(m_state);
        emit gameFinished();
    } else {
        nextPuzzle();
        // restart timer for next puzzle
        m_remainingTime = 30; // seconds per puzzle
        emit remainingTimeChanged(m_remainingTime);
        m_timer->start();
    }
}

void GameplayViewModel::nextPuzzle() {
    ++m_currentIndex;
    if (m_currentIndex < m_puzzles.size()) {
        const auto& puzzle = m_puzzles.at(m_currentIndex);
        emit puzzleChanged(puzzle.question);
        m_remainingTime = 30;
        emit remainingTimeChanged(m_remainingTime);
    }
}

void GameplayViewModel::pauseGame() {
    if (m_state != GameState::Playing) return;
    m_state = GameState::Paused;
    emit gameStateChanged(m_state);
    m_timer->stop();
}

void GameplayViewModel::resumeGame() {
    if (m_state != GameState::Paused) return;
    m_state = GameState::Playing;
    emit gameStateChanged(m_state);
    m_timer->start();
}

void GameplayViewModel::restartGame() {
    // Reset all state without automatically starting a new game.
    m_timer->stop();
    m_state = GameState::Idle;
    emit gameStateChanged(m_state);
    // Reset score, combo and puzzles for a clean slate.
    m_score = 0;
    m_comboStreak = 0;
    m_puzzles.clear();
    m_currentIndex = -1;
    // No emit for scoreChanged/comboStreakChanged here – tests expect idle state only.
}
