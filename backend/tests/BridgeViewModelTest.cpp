#include "controller/BridgeController.h"
#include "core/DatabaseManager.h"
#include "repositories/RecordRepository.h"
#include "repositories/StageRepository.h"
#include "repositories/UserRepository.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {
    void require(bool condition, const char *message) {
        if (!condition) {
            std::cerr << message << '\n';
            std::exit(1);
        }
    }

    QJsonObject parse(const QString &json) { return QJsonDocument::fromJson(json.toUtf8()).object(); }
} // namespace

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    DatabaseManager db;
    require(db.openDatabase(":memory:"), "Could not open in-memory database");
    const std::string sqlDir = WORDSPRINT_SQL_DIR;
    require(db.initializeDatabase(sqlDir + "/schema.sql", sqlDir + "/seed_app_data.sql",
                                  sqlDir + "/seed_mock_users.sql"),
            "Could not initialize test database");
    StageRepository stages(db);
    RecordRepository records(db);
    UserRepository users(db);
    BridgeController bridge(&stages, &records, &users);

    const QJsonObject profiles = parse(bridge.getProfiles("{}"));
    require(profiles.value("status") == "ok", "Profiles request failed");
    const QJsonArray rows = profiles.value("profiles").toArray();
    require(rows.size() == 8, "Unexpected profile count");
    const QJsonObject first = rows[0].toObject();
    require(first.value("score").toInt() == 528, "Best stage scores not summed correctly");
    require(first.value("progress") == "4/30", "Passing progress is incorrect");
    const QJsonArray stageRows = first.value("stages").toArray();
    require(stageRows.size() == 30, "Stage list is incomplete");
    const std::vector<int> expectedIds = {1,  2,  6,  7,  3,  4,  5,  8,  9,  10, 11, 12, 13, 14, 15,
                                          16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    for (qsizetype i = 0; i < stageRows.size(); ++i) {
        const QJsonObject stage = stageRows[i].toObject();
        const QString expectedStatus = i < 4    ? QStringLiteral("solved")
                                       : i < 15 ? QStringLiteral("available")
                                                : QStringLiteral("locked");
        require(stage.value("id").toInt() == expectedIds[static_cast<std::size_t>(i)],
                "Stages are not ordered by status, level, and ID");
        require(stage.value("status").toString() == expectedStatus, "Stage status is incorrect");
        if (stage.value("id").toInt() == 1) {
            require(stage.value("previewNextStageId").toInt() == 6, "Preview lost next level");
        } else if (stage.value("id").toInt() == 26) {
            require(stage.value("previewNextStageId").toInt() == 2, "Preview lost next topic");
        } else if (stage.value("id").toInt() == 30) {
            require(stage.value("previewNextStageId").toInt() == 0, "Last preview stage has a successor");
        }
    }
    const QJsonArray newPlayerStages = rows[6].toObject().value("stages").toArray();
    require(newPlayerStages.size() == 30, "New player stage list is incomplete");
    for (qsizetype i = 0; i < newPlayerStages.size(); ++i) {
        const QJsonObject stage = newPlayerStages[i].toObject();
        require(stage.value("id").toInt() == i + 1, "New player stage order is incorrect");
        require(stage.value("status").toString() == (i < 5 ? "available" : "locked"),
                "Stage progress leaked between profiles");
    }
    require(records.insertRecord(7, 3, 69, 1).has_value(), "Could not insert failed A1 attempt");
    auto progress = stages.getStagesWithUnlockStatus(7);
    require(progress.has_value(), "Could not load progress after failed attempt");
    for (const StageStatusDTO &stage: *progress) {
        require(stage.is_unlocked == (stage.stage.cefr_level == "A1"), "Failed attempt must not unlock another tier");
    }
    const QJsonObject a1Completion =
            parse(bridge.completeStage("{\"userId\":7,\"stageId\":3,\"score\":70,\"completionTime\":1}"));
    require(a1Completion.value("status") == "ok" && a1Completion.value("nextStageUnlocked").toBool() &&
                    a1Completion.value("nextStageId").toInt() == 8,
            "Passing A1 did not report the next tier as unlocked");
    require(records.insertRecord(7, 8, 104, 1).has_value(), "Could not insert failed A2 attempt");
    progress = stages.getStagesWithUnlockStatus(7);
    require(progress.has_value(), "Could not load progress after passing A1");
    for (const StageStatusDTO &stage: *progress) {
        require(stage.is_unlocked == (stage.stage.cefr_level == "A1" || stage.stage.cefr_level == "A2"),
                "Passing one A1 stage must unlock every A2 topic, but a failed A2 must not unlock B1");
    }
    const QJsonObject a2Completion =
            parse(bridge.completeStage("{\"userId\":7,\"stageId\":8,\"score\":105,\"completionTime\":1}"));
    require(a2Completion.value("status") == "ok" && a2Completion.value("nextStageUnlocked").toBool() &&
                    a2Completion.value("nextStageId").toInt() == 13,
            "Passing A2 did not report the next tier as unlocked");
    progress = stages.getStagesWithUnlockStatus(7);
    require(progress.has_value(), "Could not load progress after passing A2");
    for (const StageStatusDTO &stage: *progress) {
        require(stage.is_unlocked == (stage.stage.cefr_level == "A1" || stage.stage.cefr_level == "A2" ||
                                      stage.stage.cefr_level == "B1"),
                "Passing one A2 stage must unlock every B1 topic");
    }
    const std::vector<std::string> levels = {"A1", "A2", "B1", "B2", "C1", "C2"};
    const std::vector<int> higherStageIds = {11, 16, 21};
    const std::vector<int> higherPassScores = {160, 225, 330};
    for (std::size_t i = 0; i < higherStageIds.size(); ++i) {
        require(records.insertRecord(7, higherStageIds[i], higherPassScores[i], 1).has_value(),
                "Could not insert higher-tier record");
        progress = stages.getStagesWithUnlockStatus(7);
        require(progress.has_value(), "Could not load higher-tier progress");
        for (const StageStatusDTO &stage: *progress) {
            const auto level = std::find(levels.begin(), levels.end(), stage.stage.cefr_level);
            require(level != levels.end(), "Unexpected stage level");
            require(stage.is_unlocked == (static_cast<std::size_t>(level - levels.begin()) <= i + 3),
                    "Passing a higher tier must unlock every topic in the next tier only");
        }
    }
    require(records.insertRecord(1, 21, 330, 1).has_value(), "Could not insert out-of-order stage record");
    const QJsonArray reordered =
            parse(bridge.getProfiles("{}")).value("profiles").toArray()[0].toObject().value("stages").toArray();
    require(reordered[4].toObject().value("id").toInt() == 21 && reordered[4].toObject().value("status") == "solved",
            "Passed stage must be sorted as solved even if its predecessor is not passed");

    const QJsonObject leaderboard = parse(bridge.getLeaderboard("{\"stageId\":1}"));
    const QJsonArray rankings = leaderboard.value("records").toArray();
    require(rankings.size() >= 2 && rankings[0].toObject().value("player") == "Alex" &&
                    rankings[0].toObject().value("completionTime").toInt() == 25,
            "Stage rankings are incorrect");
    const QJsonObject overall = parse(bridge.getLeaderboard("{\"stageId\":null}"));
    require(overall.value("status") == "ok" &&
                    overall.value("records").toArray()[0].toObject().value("completionTime").isNull(),
            "Overall leaderboard is incorrect");
    require(parse(bridge.getLeaderboard("{}")).value("status") == "error", "Invalid leaderboard accepted");
    require(parse(bridge.createProfile("{\"name\":\"New_user\"}")).value("status") == "ok", "Creation failed");
    require(parse(bridge.createProfile("{\"name\":\"New_user\"}")).value("status") == "error",
            "Duplicate user accepted");
    require(parse(bridge.updateAudio("{\"userId\":1,\"musicEnabled\":false,\"soundEnabled\":true,"
                                     "\"musicVolume\":42,\"soundVolume\":80}"))
                            .value("status") == "ok",
            "Audio save failed");
    const QJsonObject updated = parse(bridge.getProfiles("{}")).value("profiles").toArray()[0].toObject();
    require(!updated.value("musicEnabled").toBool() && updated.value("musicVolume").toInt() == 42,
            "Audio settings did not persist");
    db.closeDatabase();
    require(parse(bridge.getProfiles("{}")).value("status") == "error", "Database failure was hidden");
    require(parse(bridge.getLeaderboard("{\"stageId\":null}")).value("status") == "error",
            "Leaderboard failure was hidden");
    return 0;
}
