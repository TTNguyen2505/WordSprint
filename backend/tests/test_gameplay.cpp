#pragma once

#include <QtTest/QtTest>
#include "../src/viewmodel/GameplayViewModel.h"
#include "../src/model/MockVocabularyRepository.h"

class GameplayTests : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void testStartAndScore();
    void testPauseResume();
    void testRestart();
private:
    GameplayViewModel* m_vm = nullptr;
    MockVocabularyRepository* m_repo = nullptr;
};

void GameplayTests::initTestCase() {
    m_repo = new MockVocabularyRepository(this);
    m_vm = new GameplayViewModel(m_repo, this);
}

void GameplayTests::testStartAndScore() {
    QSignalSpy scoreSpy(m_vm, &GameplayViewModel::scoreChanged);
    m_vm->startGame();
    // Submit correct answer for first puzzle
    const auto puzzle = m_vm->property("puzzleChanged"); // not directly accessible, use signal
    // Simulate correct answer using first puzzle's answer
    const auto puzzles = m_repo->generatePuzzles(QString(),0,10);
    QVERIFY(!puzzles.isEmpty());
    m_vm->submitAnswer(puzzles.first().answer);
    // Expect score increased
    QCOMPARE(scoreSpy.count(), 1);
    int score = m_vm->score();
    QVERIFY(score > 0);
}

void GameplayTests::testPauseResume() {
    m_vm->startGame();
    m_vm->pauseGame();
    QCOMPARE(m_vm->gameState(), GameplayViewModel::GameState::Paused);
    int timeBefore = m_vm->remainingTime();
    QTest::qWait(2000);
    QCOMPARE(m_vm->remainingTime(), timeBefore); // timer stopped
    m_vm->resumeGame();
    QCOMPARE(m_vm->gameState(), GameplayViewModel::GameState::Playing);
}

void GameplayTests::testRestart() {
    m_vm->startGame();
    m_vm->submitAnswer("wrong");
    int scoreBefore = m_vm->score();
    m_vm->restartGame();
    QCOMPARE(m_vm->score(), 0);
    QCOMPARE(m_vm->gameState(), GameplayViewModel::GameState::Idle);
    // Starting again should reset score
    m_vm->startGame();
    QCOMPARE(m_vm->score(), 0);
    QVERIFY(m_vm->score() == 0);
}

QTEST_MAIN(GameplayTests)
#include "test_gameplay.moc"
