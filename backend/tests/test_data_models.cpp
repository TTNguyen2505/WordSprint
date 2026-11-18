#include <QCoreApplication>
#include <QSignalSpy>
#include <QtTest>

#include "../src/model/WordItem.h"
#include "../src/model/Topic.h"
#include "../src/model/Stage.h"
#include "../src/model/UserProfile.h"
#include "../src/model/WordListModel.h"
#include "../src/model/TopicListModel.h"
#include "../src/model/StageListModel.h"

class TestDataModels : public QObject {
    Q_OBJECT

private slots:
    void testWordItemSerialization() {
        WordItem item;
        item.id = 42;
        item.word = QStringLiteral("Polymorphism");
        item.phonetic = QStringLiteral("/ˌpɒl.iˈmɔː.fɪ.zəm/");
        item.partOfSpeech = QStringLiteral("noun");
        item.meaningVi = QStringLiteral("Tính đa hình");
        item.definitionEn = QStringLiteral("Ability of different objects to respond to the same message.");
        item.difficultyLevel = 2;
        item.topicId = 1;

        QVERIFY(item.isValid());
        QJsonObject json = item.toJson();
        QCOMPARE(json.value(QStringLiteral("word")).toString(), QStringLiteral("Polymorphism"));

        WordItem deserialized = WordItem::fromJson(json);
        QCOMPARE(deserialized.id, 42);
        QCOMPARE(deserialized.meaningVi, QStringLiteral("Tính đa hình"));
    }

    void testWordListModelMockDataAndFilter() {
        WordListModel model;
        QVERIFY(model.rowCount() > 0);

        int initialCount = model.rowCount();
        model.filterByTopic(1);
        QVERIFY(model.rowCount() <= initialCount);
        for (int i = 0; i < model.rowCount(); ++i) {
            QCOMPARE(model.data(model.index(i), WordListModel::TopicIdRole).toInt(), 1);
        }

        model.search(QStringLiteral("Algorithm"));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0), WordListModel::WordRole).toString(), QStringLiteral("Algorithm"));

        model.resetFilter();
        QCOMPARE(model.rowCount(), initialCount);
    }

    void testTopicListModel() {
        TopicListModel model;
        QCOMPARE(model.rowCount(), 4);
        QVariantMap firstTopic = model.get(0);
        QCOMPARE(firstTopic.value(QStringLiteral("id")).toInt(), 1);
        QCOMPARE(firstTopic.value(QStringLiteral("name")).toString(), QStringLiteral("OOP & Software Architecture"));
    }

    void testStageListModelUnlockAndUpdateScore() {
        StageListModel model;
        QVERIFY(model.rowCount() >= 6);

        // Stage 3 should initially be locked
        QVariantMap stage3Before = model.get(2);
        QCOMPARE(stage3Before.value(QStringLiteral("id")).toInt(), 3);
        QCOMPARE(stage3Before.value(QStringLiteral("isUnlocked")).toBool(), false);

        model.unlockStage(3);
        QVariantMap stage3After = model.get(2);
        QCOMPARE(stage3After.value(QStringLiteral("isUnlocked")).toBool(), true);

        model.updateScore(3, 950);
        QVariantMap stage3Scored = model.get(2);
        QCOMPARE(stage3Scored.value(QStringLiteral("bestScore")).toInt(), 950);
        QCOMPARE(stage3Scored.value(QStringLiteral("isCompleted")).toBool(), true);
    }
};

QTEST_MAIN(TestDataModels)
#include "test_data_models.moc"
