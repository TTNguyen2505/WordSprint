#include "../src/database/DatabaseManager.h"

#include "../src/models/StageRecord.h"
#include "../src/models/UserProgress.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QVariant>
#include <QVariantList>

#include <optional>
#include <string>
#include <vector>

namespace {

int failureCount = 0;

void expect(bool condition, const char* description) {
    if (condition) {
        qInfo().noquote() << "PASS:" << description;
        return;
    }

    qCritical().noquote() << "FAIL:" << description;
    ++failureCount;
}

bool writeFile(const QString& path, const QByteArray& contents) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical().noquote() << "Could not create test file:" << path << file.errorString();
        return false;
    }
    return file.write(contents) == contents.size();
}

bool executeSql(
    DatabaseManager& databaseManager,
    const QString& statement,
    const QVariantList& bindings = {}) {
    const QSqlDatabase database = databaseManager.connection();
    QSqlQuery query(database);
    if (!query.prepare(statement)) {
        qCritical().noquote() << "Could not prepare test SQL:" << query.lastError().text();
        return false;
    }
    for (const QVariant& value : bindings) {
        query.addBindValue(value);
    }
    if (!query.exec()) {
        qCritical().noquote() << "Could not execute test SQL:" << query.lastError().text();
        return false;
    }
    return true;
}

std::optional<std::vector<StageRecord>> loadLeaderboard(DatabaseManager& databaseManager) {
    const QSqlDatabase database = databaseManager.connection();
    QSqlQuery query(database);
    if (!query.exec(QStringLiteral(
            "SELECT record_id, user_id, stage_id, score, completion_time, played_at "
            "FROM stage_records WHERE stage_id = 1 "
            "ORDER BY score DESC, completion_time ASC, record_id ASC"))) {
        qCritical().noquote() << "Could not read test leaderboard:" << query.lastError().text();
        return std::nullopt;
    }

    std::vector<StageRecord> records;
    while (query.next()) {
        StageRecord record;
        record.record_id = query.value(0).toInt();
        record.user_id = query.value(1).toInt();
        record.stage_id = query.value(2).toInt();
        record.score = query.value(3).toInt();
        record.completion_time = query.value(4).toInt();
        record.played_at = query.value(5).toString().toStdString();
        records.push_back(record);
    }
    return records;
}

std::optional<UserProgress> loadUserProgress(DatabaseManager& databaseManager, bool& querySucceeded) {
    querySucceeded = false;
    const QSqlDatabase database = databaseManager.connection();
    QSqlQuery query(database);
    if (!query.prepare(QStringLiteral(
            "SELECT user_id, stage_id, is_unlocked, is_completed, high_score, best_time "
            "FROM user_progress WHERE user_id = ? AND stage_id = ?"))) {
        qCritical().noquote() << "Could not prepare test progress query:" << query.lastError().text();
        return std::nullopt;
    }
    query.addBindValue(1);
    query.addBindValue(1);
    if (!query.exec()) {
        qCritical().noquote() << "Could not read test progress:" << query.lastError().text();
        return std::nullopt;
    }
    querySucceeded = true;
    if (!query.next()) {
        return std::nullopt;
    }

    UserProgress progress;
    progress.user_id = query.value(0).toInt();
    progress.stage_id = query.value(1).toInt();
    progress.is_unlocked = query.value(2).toInt();
    progress.is_completed = query.value(3).toInt();
    progress.high_score = query.value(4).toInt();
    if (!query.value(5).isNull()) {
        bool ok = false;
        const int bestTime = query.value(5).toInt(&ok);
        if (ok) {
            progress.best_time = bestTime;
        }
    }
    return progress;
}

void testUnopenedDatabase(const QString& schemaPath, const QString& seedPath) {
    DatabaseManager databaseManager;
    expect(!databaseManager.isConnected(), "new manager starts disconnected");
    expect(
        !databaseManager.initializeDatabase(schemaPath.toStdString(), seedPath.toStdString()),
        "initialization without an open connection fails safely");
    expect(!databaseManager.lastError().empty(), "unopened database reports an error");
}

void testPersistence(const QString& schemaPath, const QString& seedPath, const QString& databasePath) {
    {
        DatabaseManager databaseManager;
        const bool opened = databaseManager.openDatabase(databasePath.toStdString());
        expect(opened, "new file database opens");
        if (!opened) {
            return;
        }

        const bool initialized = databaseManager.initializeDatabase(schemaPath.toStdString(), seedPath.toStdString());
        expect(initialized, "schema initializes on a new database");
        if (!initialized) {
            return;
        }

        const auto emptyRecords = loadLeaderboard(databaseManager);
        expect(emptyRecords.has_value() && emptyRecords->empty(), "empty leaderboard returns an empty result");

        bool progressQuerySucceeded = false;
        const auto emptyProgress = loadUserProgress(databaseManager, progressQuerySucceeded);
        expect(progressQuerySucceeded && !emptyProgress.has_value(), "missing unlock state returns no row safely");

        const bool insertedUser = executeSql(
            databaseManager,
            QStringLiteral("INSERT INTO users (user_id, username) VALUES (?, ?)"),
            {QVariant(1), QVariant(QStringLiteral("Persistence Test"))});
        const bool insertedTopic = executeSql(
            databaseManager,
            QStringLiteral("INSERT INTO topics (topic_id, topic_name) VALUES (?, ?)"),
            {QVariant(1), QVariant(QStringLiteral("Persistence Topic"))});
        const bool insertedStage = executeSql(
            databaseManager,
            QStringLiteral("INSERT INTO stages (stage_id, topic_id, cefr_level, unlock_order) VALUES (?, ?, ?, ?)"),
            {QVariant(1), QVariant(1), QVariant(QStringLiteral("A1")), QVariant(1)});
        expect(insertedUser && insertedTopic && insertedStage, "test user, topic, and stage are created");
        if (!insertedUser || !insertedTopic || !insertedStage) {
            return;
        }

        const bool firstRecord = executeSql(
            databaseManager,
            QStringLiteral("INSERT INTO stage_records (user_id, stage_id, score, completion_time) VALUES (?, ?, ?, ?)"),
            {QVariant(1), QVariant(1), QVariant(90), QVariant(42)});
        const bool secondRecord = executeSql(
            databaseManager,
            QStringLiteral("INSERT INTO stage_records (user_id, stage_id, score, completion_time) VALUES (?, ?, ?, ?)"),
            {QVariant(1), QVariant(1), QVariant(90), QVariant(37)});
        const bool thirdRecord = executeSql(
            databaseManager,
            QStringLiteral("INSERT INTO stage_records (user_id, stage_id, score, completion_time) VALUES (?, ?, ?, ?)"),
            {QVariant(1), QVariant(1), QVariant(75), QVariant(30)});
        const bool insertedProgress = executeSql(
            databaseManager,
            QStringLiteral(
                "INSERT INTO user_progress "
                "(user_id, stage_id, is_unlocked, is_completed, high_score, best_time) "
                "VALUES (?, ?, ?, ?, ?, ?)"),
            {QVariant(1), QVariant(1), QVariant(1), QVariant(1), QVariant(90), QVariant(37)});
        expect(
            firstRecord && secondRecord && thirdRecord && insertedProgress,
            "leaderboard and unlock state are saved");
    }

    {
        DatabaseManager databaseManager;
        const bool reopened = databaseManager.openDatabase(databasePath.toStdString());
        expect(reopened, "same file database reopens after manager destruction");
        if (!reopened) {
            return;
        }

        const auto records = loadLeaderboard(databaseManager);
        expect(records.has_value(), "leaderboard query succeeds after restart");
        if (records.has_value()) {
            expect(records->size() == 3, "all leaderboard records persist after restart");
            if (records->size() == 3) {
                expect(
                    (*records)[0].score == 90 && (*records)[0].completion_time == 37 &&
                        (*records)[1].score == 90 && (*records)[1].completion_time == 42 &&
                        (*records)[2].score == 75 && (*records)[2].completion_time == 30,
                    "leaderboard scores and completion times persist in rank order");
            }
        }

        bool progressQuerySucceeded = false;
        const auto progress = loadUserProgress(databaseManager, progressQuerySucceeded);
        expect(progressQuerySucceeded && progress.has_value(), "unlock state exists after restart");
        if (progress.has_value()) {
            expect(
                progress->is_unlocked == 1 && progress->is_completed == 1 && progress->high_score == 90 &&
                    progress->best_time == 37,
                "unlock flags and progress values persist after restart");
        }
    }
}

void testMissingSchemaAndInvalidDatabase(
    const QString& schemaPath,
    const QString& seedPath,
    const QString& temporaryPath) {
    const QString missingSchemaPath = QDir(temporaryPath).filePath(QStringLiteral("missing-schema.sql"));
    const QString emptyDatabasePath = QDir(temporaryPath).filePath(QStringLiteral("empty.sqlite"));
    {
        DatabaseManager databaseManager;
        const bool opened = databaseManager.openDatabase(emptyDatabasePath.toStdString());
        expect(opened, "empty database file opens without crashing");
        if (opened) {
            expect(
                !databaseManager.initializeDatabase(missingSchemaPath.toStdString(), seedPath.toStdString()),
                "missing schema fails safely");
            expect(!databaseManager.lastError().empty(), "missing schema reports an error");
        }
    }

    const QString invalidDatabasePath = QDir(temporaryPath).filePath(QStringLiteral("invalid.sqlite"));
    expect(
        writeFile(invalidDatabasePath, QByteArrayLiteral("not a SQLite database")),
        "invalid database fixture is written");
    DatabaseManager invalidDatabaseManager;
    const bool opened = invalidDatabaseManager.openDatabase(invalidDatabasePath.toStdString());
    const bool initialized = opened &&
        invalidDatabaseManager.initializeDatabase(schemaPath.toStdString(), seedPath.toStdString());
    expect(!opened || !initialized, "invalid database content is rejected safely");
    expect(!invalidDatabaseManager.lastError().empty(), "invalid database reports an error");
}

}  // namespace

int main(int argc, char* argv[]) {
    QCoreApplication application(argc, argv);
    if (application.arguments().size() != 2) {
        qCritical() << "Usage: DatabasePersistenceTests <schema.sql>";
        return 2;
    }

    QTemporaryDir temporaryDirectory;
    expect(temporaryDirectory.isValid(), "temporary test directory is available");
    if (!temporaryDirectory.isValid()) {
        return 2;
    }

    const QString temporaryPath = temporaryDirectory.path();
    const QString schemaPath = application.arguments().at(1);
    const QString seedPath = QDir(temporaryPath).filePath(QStringLiteral("empty-seed.sql"));
    const QString databasePath = QDir(temporaryPath).filePath(QStringLiteral("persistence.sqlite"));
    if (!writeFile(seedPath, QByteArray{})) {
        return 2;
    }

    testUnopenedDatabase(schemaPath, seedPath);
    testPersistence(schemaPath, seedPath, databasePath);
    testMissingSchemaAndInvalidDatabase(schemaPath, seedPath, temporaryPath);

    if (failureCount != 0) {
        qCritical() << failureCount << "database persistence test(s) failed.";
        return 1;
    }
    qInfo() << "All database persistence tests passed.";
    return 0;
}
