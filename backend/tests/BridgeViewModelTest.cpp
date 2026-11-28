#include "controller/BridgeController.h"
#include "core/DatabaseManager.h"
#include "repositories/RecordRepository.h"
#include "repositories/StageRepository.h"
#include "repositories/UserRepository.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

namespace {
    constexpr int sampleUserId = 1;
    constexpr int newUserId = 2;
    constexpr int sampleTopicId = 1;
    constexpr int otherTopicId = 2;
    constexpr int a1StageId = 1;
    constexpr int a2StageId = 2;
    constexpr int b1StageId = 3;
    constexpr int b2StageId = 4;
    constexpr int c1StageId = 5;
    constexpr int c2StageId = 6;
    constexpr int otherA1StageId = 7;
    constexpr int otherA2StageId = 8;
    constexpr int a1PassScore = 70;
    constexpr int a2PassScore = 105;
    constexpr int b1PassScore = 160;
    constexpr int b2PassScore = 225;
    constexpr int c1PassScore = 330;
    constexpr int sampleA1BestScore = 86;
    constexpr int sampleA1BestTime = 38;
    constexpr int sampleA2BestScore = 111;
    constexpr int sampleB1BestScore = 145;
    constexpr int sampleSolvedCount = 2;
    constexpr int completionTime = 1;
    constexpr int tieBreakTime = 25;
    constexpr int musicVolume = 42;
    constexpr int soundVolume = 80;
    constexpr std::array<int, 4> seededStageIds = {a1StageId, a2StageId, b1StageId, b2StageId};
    constexpr std::array<const char *, 6> levels = {"A1", "A2", "B1", "B2", "C1", "C2"};

    void require(bool condition, const char *message) {
        if (!condition) {
            std::cerr << message << '\n';
            std::exit(1);
        }
    }

    QJsonObject parse(const QString &json) { return QJsonDocument::fromJson(json.toUtf8()).object(); }

    QString request(const QJsonObject &object) {
        return QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Compact));
    }

    void addStage(DatabaseManager &database, int id, int topicId, const char *level, int passScore) {
        QSqlQuery query(database.connection());
        require(query.prepare(QStringLiteral("INSERT INTO stages "
                                             "(stage_id, topic_id, cefr_level, time_limit, question_count, pass_score) "
                                             "VALUES (?, ?, ?, ?, ?, ?)")),
                "Could not prepare test stage");
        constexpr int timeLimit = 60;
        constexpr int questionCount = 1;
        query.addBindValue(id);
        query.addBindValue(topicId);
        query.addBindValue(QString::fromLatin1(level));
        query.addBindValue(timeLimit);
        query.addBindValue(questionCount);
        query.addBindValue(passScore);
        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            std::exit(1);
        }
    }
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
    constexpr int seededUserCount = 2;
    require(rows.size() == seededUserCount, "Unexpected profile count");
    const QJsonObject first = rows[0].toObject();
    require(first.value("score").toInt() == sampleA1BestScore + sampleA2BestScore + sampleB1BestScore,
            "Best stage scores not summed correctly");
    const auto sampleBestRecord = records.getBestRecord(sampleUserId, a1StageId);
    require(sampleBestRecord.has_value() && sampleBestRecord->score == sampleA1BestScore &&
                    sampleBestRecord->completion_time == sampleA1BestTime,
            "Best stage record did not use completion time to break the score tie");
    require(first.value("progress").toString() ==
                    QStringLiteral("%1/%2").arg(sampleSolvedCount).arg(seededStageIds.size()),
            "Passing progress is incorrect");
    const QJsonArray stageRows = first.value("stages").toArray();
    require(stageRows.size() == seededStageIds.size(), "Stage list is incomplete");
    for (qsizetype i = 0; i < stageRows.size(); ++i) {
        const QJsonObject stage = stageRows[i].toObject();
        const QString expectedStatus = i < sampleSolvedCount    ? QStringLiteral("solved")
                                       : i == sampleSolvedCount ? QStringLiteral("available")
                                                                : QStringLiteral("locked");
        require(stage.value("id").toInt() == seededStageIds[static_cast<std::size_t>(i)],
                "Stages are not ordered by status and level");
        require(stage.value("status").toString() == expectedStatus, "Stage status is incorrect");
        const int expectedNext = i + 1 < stageRows.size() ? seededStageIds[static_cast<std::size_t>(i + 1)] : 0;
        require(stage.value("previewNextStageId").toInt() == expectedNext, "Preview navigation is incorrect");
    }
    const QJsonArray newPlayerStages = rows[1].toObject().value("stages").toArray();
    require(newPlayerStages.size() == seededStageIds.size(), "New player stage list is incomplete");
    for (qsizetype i = 0; i < newPlayerStages.size(); ++i) {
        const QJsonObject stage = newPlayerStages[i].toObject();
        require(stage.value("id").toInt() == seededStageIds[static_cast<std::size_t>(i)],
                "New player stage order is incorrect");
        require(stage.value("status").toString() == (i == 0 ? "available" : "locked"),
                "Stage progress leaked between profiles");
    }

    constexpr std::array<int, 4> expectedQuestionCounts = {10, 10, 12, 12};
    constexpr std::array<int, 4> expectedTimeLimits = {90, 100, 120, 135};
    require(db.connection().transaction(), "Could not start seed validation transaction");
    require(records.insertRecord(sampleUserId, b1StageId, b1PassScore, completionTime).has_value(),
            "Could not unlock B2 for seed validation");
    for (std::size_t i = 0; i < seededStageIds.size(); ++i) {
        QSqlQuery pool(db.connection());
        require(pool.prepare(QStringLiteral("SELECT COUNT(*), COUNT(audio_path) FROM word_pool "
                                            "WHERE topic_id = ? AND cefr_level = ?")),
                "Could not prepare word pool query");
        pool.addBindValue(sampleTopicId);
        pool.addBindValue(QString::fromLatin1(levels[i]));
        require(pool.exec() && pool.next(), "Could not query word pool");
        const int wordCountForLevel = pool.value(0).toInt();
        const int audioCount = pool.value(1).toInt();
        require(wordCountForLevel >= expectedQuestionCounts[i] && audioCount == wordCountForLevel,
                "Each stage needs enough words, and each seeded word must have an audio path");

        const QJsonObject started =
                parse(bridge.startStage(request({{"userId", sampleUserId}, {"stageId", seededStageIds[i]}})));
        const QJsonArray words = started.value("words").toArray();
        const auto stage = stages.getStageById(seededStageIds[i]);
        require(stage.has_value() && stage->question_count == expectedQuestionCounts[i],
                "Seeded stage has an incorrect question count");
        require(stage->time_limit == expectedTimeLimits[i] &&
                        started.value("timeLimit").toInt() == expectedTimeLimits[i],
                "Seeded stage has an incorrect time limit");
        require(started.value("passScore").toInt(-1) == stage->pass_score, "Stage pass score was not returned");
        require(started.value("status") == "ok" && words.size() == expectedQuestionCounts[i] &&
                        started.value("questionCount").toInt() == expectedQuestionCounts[i],
                "Seeded stage returned an incorrect number of questions");
        for (const QJsonValue &value: words) {
            const QJsonObject word = value.toObject();
            require(word.value("cefr_level").toString() == QString::fromLatin1(levels[i]),
                    "Stage returned a word from the wrong level");
            QSqlQuery selectedWord(db.connection());
            require(selectedWord.prepare(QStringLiteral("SELECT audio_path FROM word_pool WHERE word_id = ?")),
                    "Could not prepare selected word query");
            selectedWord.addBindValue(word.value("word_id").toInt());
            require(selectedWord.exec() && selectedWord.next(), "Could not query selected word");
            require(!selectedWord.value(0).isNull(), "Seeded word has no audio path");
            if (selectedWord.value(0).toString() == QStringLiteral("storage/audio/hello.mp3")) {
                require(word.value("audioFallbackStatus") == "available" &&
                                word.value("audio_path").toString() == selectedWord.value(0).toString(),
                        "Embedded audio path was not returned");
            } else {
                require(word.value("audioFallbackStatus") == "not_found" && word.value("audio_path").isNull(),
                        "Unembedded audio path should not be playable");
            }
        }
    }
    QSqlQuery audioChanges(db.connection());
    const auto startA1 = [&bridge]() {
        return parse(bridge.startStage(request({{"userId", sampleUserId}, {"stageId", a1StageId}})));
    };
    for (const auto &test: std::array<std::pair<const char *, const char *>, 3>{
                 {{"NULL", "missing"}, {"'storage/audio/hello.wav'", "not_found"},
                  {"'../audio/hello.mp3'", "invalid_path"}}}) {
        require(audioChanges.exec(QStringLiteral("UPDATE word_pool SET audio_path = %1 WHERE cefr_level = 'A1'")
                                          .arg(QString::fromLatin1(test.first))),
                "Could not update audio fallback for validation");
        const QJsonObject result = startA1();
        require(result.value("status") == "ok", "Could not start stage for audio fallback validation");
        for (const QJsonValue &value: result.value("words").toArray()) {
            require(value.toObject().value("audioFallbackStatus") == QString::fromLatin1(test.second) &&
                            value.toObject().value("audioFallbackUrl").toString().isEmpty() &&
                            value.toObject().value("audio_path").isNull(),
                    "Audio fallback status was not reported correctly");
        }
    }
    require(audioChanges.exec(QStringLiteral("UPDATE word_pool SET target_word = '  ' WHERE cefr_level = 'A1'")),
            "Could not clear target words for validation");
    require(startA1().value("status") == "error", "A stage without valid words was accepted");
    require(db.connection().rollback(), "Could not roll back seed validation");

    // Additional stages keep cross-topic and C1/C2 progression covered independently of the small seed.
    QSqlQuery topicQuery(db.connection());
    require(topicQuery.prepare(QStringLiteral("INSERT INTO topics (topic_id, topic_name) VALUES (?, ?)")),
            "Could not prepare test topic");
    topicQuery.addBindValue(otherTopicId);
    topicQuery.addBindValue(QStringLiteral("Test topic"));
    require(topicQuery.exec(), "Could not insert test topic");
    constexpr int c2PassScore = 385;
    addStage(db, c1StageId, sampleTopicId, levels[4], c1PassScore);
    addStage(db, c2StageId, sampleTopicId, levels[5], c2PassScore);
    addStage(db, otherA1StageId, otherTopicId, levels[0], a1PassScore);
    addStage(db, otherA2StageId, otherTopicId, levels[1], a2PassScore);

    require(records.insertRecord(newUserId, a1StageId, a1PassScore - 1, completionTime).has_value(),
            "Could not insert failed A1 attempt");
    auto progress = stages.getStagesWithUnlockStatus(newUserId);
    require(progress.has_value(), "Could not load progress after failed attempt");
    for (const StageStatusDTO &stage: *progress) {
        require(stage.is_unlocked == (stage.stage.cefr_level == levels[0]),
                "Failed attempt must not unlock another tier");
    }
    const auto complete = [&bridge](int userId, int stageId, int score) {
        return parse(bridge.completeStage(request(
                {{"userId", userId}, {"stageId", stageId}, {"score", score}, {"completionTime", completionTime}})));
    };
    const QJsonObject a1Completion = complete(newUserId, a1StageId, a1PassScore);
    require(a1Completion.value("status") == "ok" && a1Completion.value("nextStageUnlocked").toBool() &&
                    a1Completion.value("nextStageId").toInt() == a2StageId,
            "Passing A1 did not report the next tier as unlocked");
    require(records.insertRecord(newUserId, a2StageId, a2PassScore - 1, completionTime).has_value(),
            "Could not insert failed A2 attempt");
    progress = stages.getStagesWithUnlockStatus(newUserId);
    require(progress.has_value(), "Could not load progress after passing A1");
    for (const StageStatusDTO &stage: *progress) {
        require(stage.is_unlocked == (stage.stage.cefr_level == levels[0] || stage.stage.cefr_level == levels[1]),
                "Passing one A1 stage must unlock every A2 topic, but a failed A2 must not unlock B1");
    }
    const QJsonObject a2Completion = complete(newUserId, a2StageId, a2PassScore);
    require(a2Completion.value("status") == "ok" && a2Completion.value("nextStageUnlocked").toBool() &&
                    a2Completion.value("nextStageId").toInt() == b1StageId,
            "Passing A2 did not report the next tier as unlocked");
    const std::array<int, 3> higherStageIds = {b1StageId, b2StageId, c1StageId};
    const std::array<int, 3> higherPassScores = {b1PassScore, b2PassScore, c1PassScore};
    for (std::size_t i = 0; i <= higherStageIds.size(); ++i) {
        progress = stages.getStagesWithUnlockStatus(newUserId);
        require(progress.has_value(), "Could not load higher-tier progress");
        for (const StageStatusDTO &stage: *progress) {
            const auto level = std::find(levels.begin(), levels.end(), stage.stage.cefr_level);
            require(level != levels.end(), "Unexpected stage level");
            require(stage.is_unlocked == (static_cast<std::size_t>(level - levels.begin()) <= i + 2),
                    "Passing a tier must unlock every topic in the next tier only");
        }
        if (i < higherStageIds.size()) {
            require(records.insertRecord(newUserId, higherStageIds[i], higherPassScores[i], completionTime).has_value(),
                    "Could not insert higher-tier record");
        }
    }
    require(records.insertRecord(sampleUserId, c1StageId, c1PassScore, completionTime).has_value(),
            "Could not insert out-of-order stage record");
    const QJsonArray reordered =
            parse(bridge.getProfiles("{}")).value("profiles").toArray()[0].toObject().value("stages").toArray();
    require(reordered[sampleSolvedCount].toObject().value("id").toInt() == c1StageId &&
                    reordered[sampleSolvedCount].toObject().value("status") == "solved",
            "Passed stage must be sorted as solved even if its predecessor is not passed");
    for (const QJsonValue &value: reordered) {
        const QJsonObject stage = value.toObject();
        if (stage.value("id").toInt() == c2StageId) {
            require(stage.value("previewNextStageId").toInt() == otherA1StageId, "Preview lost next topic");
        } else if (stage.value("id").toInt() == otherA2StageId) {
            require(stage.value("previewNextStageId").toInt() == 0, "Last preview stage has a successor");
        }
    }

    require(records.insertRecord(newUserId, a1StageId, sampleA1BestScore, tieBreakTime).has_value(),
            "Could not insert tied leaderboard record");
    const QJsonObject leaderboard = parse(bridge.getLeaderboard(request({{"stageId", a1StageId}})));
    const QJsonArray rankings = leaderboard.value("records").toArray();
    require(rankings.size() >= seededUserCount && rankings[0].toObject().value("player") == "Alex" &&
                    rankings[0].toObject().value("completionTime").toInt() == tieBreakTime,
            "Stage rankings are incorrect");
    const QJsonObject overall = parse(bridge.getLeaderboard(request({{"stageId", QJsonValue::Null}})));
    require(overall.value("status") == "ok" &&
                    overall.value("records").toArray()[0].toObject().value("completionTime").isNull(),
            "Overall leaderboard is incorrect");
    require(parse(bridge.getLeaderboard("{}")).value("status") == "error", "Invalid leaderboard accepted");
    require(parse(bridge.createProfile("{\"name\":\"New_user\"}")).value("status") == "ok", "Creation failed");
    require(parse(bridge.createProfile("{\"name\":\"New_user\"}")).value("status") == "error",
            "Duplicate user accepted");
    require(parse(bridge.updateAudio(request({{"userId", sampleUserId},
                                              {"musicEnabled", false},
                                              {"soundEnabled", true},
                                              {"musicVolume", musicVolume},
                                              {"soundVolume", soundVolume}})))
                            .value("status") == "ok",
            "Audio save failed");
    const QJsonObject updated = parse(bridge.getProfiles("{}")).value("profiles").toArray()[0].toObject();
    require(!updated.value("musicEnabled").toBool() && updated.value("musicVolume").toInt() == musicVolume,
            "Audio settings did not persist");
    db.closeDatabase();
    require(parse(bridge.getProfiles("{}")).value("status") == "error", "Database failure was hidden");
    require(parse(bridge.getLeaderboard(request({{"stageId", QJsonValue::Null}}))).value("status") == "error",
            "Leaderboard failure was hidden");
    return 0;
}
