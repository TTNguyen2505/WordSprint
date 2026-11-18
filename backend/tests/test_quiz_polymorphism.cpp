#include <QCoreApplication>
#include <QtTest>
#include <vector>
#include <memory>

#include "../src/model/WordItem.h"
#include "../src/quiz/QuizBase.h"
#include "../src/quiz/ScrambleQuiz.h"
#include "../src/quiz/FillInBlankQuiz.h"
#include "../src/quiz/ListeningQuiz.h"

class TestQuizPolymorphism : public QObject {
    Q_OBJECT

private:
    WordItem m_sampleWord;

private slots:
    void initTestCase() {
        m_sampleWord.id = 1;
        m_sampleWord.word = QStringLiteral("INHERITANCE");
        m_sampleWord.phonetic = QStringLiteral("/ɪnˈher.ɪ.təns/");
        m_sampleWord.partOfSpeech = QStringLiteral("noun");
        m_sampleWord.meaningVi = QStringLiteral("Tính kế thừa");
        m_sampleWord.definitionEn = QStringLiteral("Mechanism where a new class acquires properties of an existing class.");
        m_sampleWord.exampleSentence = QStringLiteral("Inheritance facilitates code reuse.");
        m_sampleWord.audioPath = QStringLiteral("assets/audio/inheritance.mp3");
    }

    void testPolymorphicVectorExecution() {
        std::vector<std::unique_ptr<QuizBase>> quizList;
        quizList.push_back(std::make_unique<ScrambleQuiz>(1, m_sampleWord));
        quizList.push_back(std::make_unique<FillInBlankQuiz>(2, m_sampleWord));
        quizList.push_back(std::make_unique<ListeningQuiz>(3, m_sampleWord));

        QCOMPARE(static_cast<int>(quizList.size()), 3);

        // Verify runtime type identification via polymorphism
        QCOMPARE(quizList[0]->getType(), QuizType::Scramble);
        QCOMPARE(quizList[0]->getTypeString(), QStringLiteral("Scramble"));

        QCOMPARE(quizList[1]->getType(), QuizType::FillInBlank);
        QCOMPARE(quizList[1]->getTypeString(), QStringLiteral("FillInBlank"));

        QCOMPARE(quizList[2]->getType(), QuizType::Listening);
        QCOMPARE(quizList[2]->getTypeString(), QStringLiteral("Listening"));

        // Verify uniform answer verification through base pointer
        for (auto &quiz : quizList) {
            // Wrong answer test
            QVERIFY(!quiz->checkAnswer(QStringLiteral("WRONG_ANSWER")));
            QVERIFY(!quiz->isSolved());
            QCOMPARE(quiz->calculateScore(25), 0);

            // Correct answer test
            QVERIFY(quiz->checkAnswer(QStringLiteral("inheritance")));
            QVERIFY(quiz->isSolved());
            QVERIFY(quiz->calculateScore(25) > 0);
        }
    }

    void testFillInBlankAlternateMissingAnswer() {
        FillInBlankQuiz quiz(10, m_sampleWord);
        QVERIFY(!quiz.blankIndices().isEmpty());

        QString expectedMissing;
        QString target = m_sampleWord.word.toUpper();
        for (int idx : quiz.blankIndices()) {
            expectedMissing.append(target[idx]);
        }

        // Check answering with only missing letters
        QVERIFY(quiz.checkAnswer(expectedMissing));
        QVERIFY(quiz.isSolved());
    }

    void testScrambleProperty() {
        ScrambleQuiz quiz(20, m_sampleWord);
        QVERIFY(!quiz.scrambledWord().isEmpty());
        QCOMPARE(quiz.scrambledWord().length(), m_sampleWord.word.length());
        QVERIFY(quiz.checkAnswer(QStringLiteral("inheritance")));
    }
};

QTEST_MAIN(TestQuizPolymorphism)
#include "test_quiz_polymorphism.moc"
