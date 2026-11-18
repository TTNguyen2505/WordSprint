#include <QCoreApplication>
#include <QtTest>
#include <memory>

#include "../src/model/WordItem.h"
#include "../src/quiz/QuizFactory.h"
#include "../src/quiz/QuizBase.h"
#include "../src/hint/HintManager.h"
#include "../src/hint/RevealLetterHintStrategy.h"
#include "../src/hint/FreezeTimeHintStrategy.h"
#include "../src/hint/ShowImageHintStrategy.h"

// Custom Strategy demonstrating Open/Closed Principle
class HalfChoiceHintStrategy : public IHintStrategy {
public:
    HintType getType() const override { return HintType::Custom; }
    QString getName() const override { return QStringLiteral("50:50 Loại trừ"); }
    QString getDescription() const override { return QStringLiteral("Loại bớt các phương án sai"); }
    int getCost() const override { return 30; }

    HintResult execute(QuizBase *quiz, GameContext &context) override {
        Q_UNUSED(quiz);
        HintResult res;
        res.success = true;
        res.type = HintType::Custom;
        res.costDeducted = getCost();
        res.message = QStringLiteral("Đã loại trừ 2 phương án sai.");
        context.deductScore(getCost());
        return res;
    }
};

class TestHintStrategy : public QObject {
    Q_OBJECT

private:
    WordItem m_word;
    std::unique_ptr<QuizBase> m_quiz;

private slots:
    void initTestCase() {
        m_word.id = 55;
        m_word.word = QStringLiteral("STRATEGY");
        m_word.meaningVi = QStringLiteral("Chiến lược");
        m_word.hintImage = QStringLiteral("assets/images/strategy.png");
        m_quiz = QuizFactory::create(1, QuizType::Scramble, m_word);
    }

    void testRevealLetterStrategy() {
        HintManager manager;
        GameContext context;
        context.totalScore = 100;

        HintResult r1 = manager.applyHint(HintType::RevealLetter, m_quiz.get(), context);
        QVERIFY(r1.success);
        QCOMPARE(r1.costDeducted, 20);
        QCOMPARE(context.totalScore, 80);
        QCOMPARE(context.revealedIndices.size(), 1);
        QCOMPARE(r1.payload.value(QStringLiteral("character")).toString(), QStringLiteral("S"));

        HintResult r2 = manager.applyHint(HintType::RevealLetter, m_quiz.get(), context);
        QVERIFY(r2.success);
        QCOMPARE(context.revealedIndices.size(), 2);
        QCOMPARE(r2.payload.value(QStringLiteral("character")).toString(), QStringLiteral("T"));
    }

    void testFreezeTimeStrategy() {
        HintManager manager;
        GameContext context;
        context.totalScore = 100;
        context.remainingSeconds = 30;

        HintResult r = manager.applyHint(HintType::FreezeTime, m_quiz.get(), context);
        QVERIFY(r.success);
        QCOMPARE(r.costDeducted, 15);
        QCOMPARE(context.totalScore, 85);
        QVERIFY(context.isTimeFrozen);
        QCOMPARE(context.remainingSeconds, 40);
        QCOMPARE(context.freezeBonusSeconds, 10);
    }

    void testShowImageStrategy() {
        HintManager manager;
        GameContext context;
        context.totalScore = 50;

        HintResult r = manager.applyHint(HintType::ShowImage, m_quiz.get(), context);
        QVERIFY(r.success);
        QCOMPARE(r.costDeducted, 10);
        QCOMPARE(context.totalScore, 40);
        QCOMPARE(r.payload.value(QStringLiteral("imagePath")).toString(), QStringLiteral("assets/images/strategy.png"));
    }

    void testInsufficientScore() {
        HintManager manager;
        GameContext context;
        context.totalScore = 5; // Less than cost of 20

        HintResult r = manager.applyHint(HintType::RevealLetter, m_quiz.get(), context);
        QVERIFY(!r.success);
        QCOMPARE(r.costDeducted, 0);
        QCOMPARE(context.totalScore, 5);
    }

    void testOpenClosedPrincipleRuntimeExtension() {
        HintManager manager;
        GameContext context;
        context.totalScore = 100;

        // Custom strategy registered dynamically at runtime without modifying HintManager
        manager.registerStrategy(std::make_unique<HalfChoiceHintStrategy>());
        QVERIFY(manager.hasStrategy(HintType::Custom));

        HintResult r = manager.applyHint(HintType::Custom, m_quiz.get(), context);
        QVERIFY(r.success);
        QCOMPARE(r.costDeducted, 30);
        QCOMPARE(context.totalScore, 70);
    }
};

QTEST_MAIN(TestHintStrategy)
#include "test_hint_strategy.moc"
