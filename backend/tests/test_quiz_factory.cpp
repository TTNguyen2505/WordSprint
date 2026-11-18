#include <QCoreApplication>
#include <QtTest>
#include <vector>
#include <memory>

#include "../src/model/WordItem.h"
#include "../src/quiz/QuizFactory.h"
#include "../src/quiz/QuizBase.h"

class TestQuizFactory : public QObject {
    Q_OBJECT

private:
    WordItem m_word;

private slots:
    void initTestCase() {
        m_word.id = 101;
        m_word.word = QStringLiteral("FACTORY");
        m_word.phonetic = QStringLiteral("/ˈfæk.tər.i/");
        m_word.meaningVi = QStringLiteral("Nhà máy / Mẫu khởi tạo");
        m_word.definitionEn = QStringLiteral("A design pattern that creates objects without specifying the exact class.");
        m_word.audioPath = QStringLiteral("assets/audio/factory.mp3");
    }

    void testFactoryCreationByType() {
        std::unique_ptr<QuizBase> scramble = QuizFactory::create(1, QuizType::Scramble, m_word);
        QVERIFY(scramble != nullptr);
        QCOMPARE(scramble->getType(), QuizType::Scramble);
        QCOMPARE(scramble->id(), 1);

        std::unique_ptr<QuizBase> fillIn = QuizFactory::create(2, QuizType::FillInBlank, m_word);
        QVERIFY(fillIn != nullptr);
        QCOMPARE(fillIn->getType(), QuizType::FillInBlank);
        QCOMPARE(fillIn->id(), 2);

        std::unique_ptr<QuizBase> listening = QuizFactory::create(3, QuizType::Listening, m_word);
        QVERIFY(listening != nullptr);
        QCOMPARE(listening->getType(), QuizType::Listening);
        QCOMPARE(listening->id(), 3);
    }

    void testBatchAndRandomCreation() {
        std::vector<WordItem> wordList = {m_word, m_word, m_word, m_word, m_word};

        auto quizSet = QuizFactory::createStageQuizzes(wordList, QuizType::Mixed);
        QCOMPARE(static_cast<int>(quizSet.size()), 5);

        for (int i = 0; i < 5; ++i) {
            QVERIFY(quizSet[i] != nullptr);
            QCOMPARE(quizSet[i]->id(), i + 1);
            QVERIFY(quizSet[i]->getType() == QuizType::Scramble ||
                     quizSet[i]->getType() == QuizType::FillInBlank ||
                     quizSet[i]->getType() == QuizType::Listening);
        }
    }

    void testDecoupling() {
        // ViewModel only deals with unique_ptr<QuizBase>
        std::unique_ptr<QuizBase> quiz = QuizFactory::createRandom(99, m_word);
        QVERIFY(quiz->checkAnswer(QStringLiteral("factory")));
        QVERIFY(quiz->isSolved());
    }
};

QTEST_MAIN(TestQuizFactory)
#include "test_quiz_factory.moc"
