#include "core/DatabaseManager.h"

#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
    void require(bool condition, const char *message) {
        if (!condition) {
            std::cerr << message << '\n';
            std::exit(1);
        }
    }

    void execute(DatabaseManager &database, const QString &sql) {
        QSqlQuery query(database.connection());
        if (!query.exec(sql)) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            std::exit(1);
        }
    }

    int scalar(DatabaseManager &database, const QString &sql) {
        QSqlQuery query(database.connection());
        if (!query.exec(sql) || !query.next()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            std::exit(1);
        }
        return query.value(0).toInt();
    }
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    const std::string sqlDir = WORDSPRINT_SQL_DIR;
    const std::string schema = sqlDir + "/schema.sql";
    const std::string appSeed = sqlDir + "/seed_app_data.sql";
    const std::string mockSeed = sqlDir + "/seed_mock_users.sql";

    DatabaseManager database;
    require(database.openDatabase(":memory:"), "Could not open in-memory database");
    execute(database, QStringLiteral("CREATE TABLE legacy_data (value INTEGER)"));
    execute(database, QStringLiteral("INSERT INTO legacy_data VALUES (42)"));

    require(!database.initializeDatabase(schema, appSeed, sqlDir + "/missing_seed.sql"),
            "Initialization should fail when a seed file is missing");
    require(!database.lastError().empty(), "Initialization failure did not provide an error");
    require(scalar(database, QStringLiteral("SELECT value FROM legacy_data")) == 42,
            "Failed initialization did not restore the old database");
    require(scalar(database, QStringLiteral("PRAGMA user_version")) == 0,
            "Failed initialization changed the database version");
    require(scalar(database, QStringLiteral("PRAGMA foreign_keys")) == 1,
            "Failed initialization did not restore foreign keys");

    require(database.initializeDatabase(schema, appSeed, mockSeed), "Database initialization failed");
    require(scalar(database, QStringLiteral("SELECT COUNT(*) FROM sqlite_master WHERE name = 'legacy_data'")) == 0,
            "Initialization did not remove the legacy schema");
    require(scalar(database, QStringLiteral("SELECT COUNT(*) FROM users")) > 0,
            "Mock users were not seeded");
    require(scalar(database, QStringLiteral("SELECT COUNT(*) FROM stages")) > 0,
            "Stages were not seeded");
    require(scalar(database, QStringLiteral("PRAGMA user_version")) == 1, "Database version was not set");
    require(scalar(database, QStringLiteral("PRAGMA foreign_keys")) == 1, "Foreign keys are disabled");

    execute(database, QStringLiteral("INSERT INTO users (username) VALUES ('Persistent_user')"));
    require(database.initializeDatabase(schema, appSeed, mockSeed), "Reinitialization failed");
    require(scalar(database, QStringLiteral("SELECT COUNT(*) FROM users WHERE username = 'Persistent_user'")) == 1,
            "Reinitialization erased existing data");
    return 0;
}
