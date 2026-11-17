#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[]) {
    // Utility to execute a multi‑statement SQL script safely
    auto executeSqlScript = [&](QSqlDatabase &db, const QString &script) -> bool {
        QSqlQuery query(db);
        QString stmt;
        bool inString = false;
        const int len = script.length();
        for (int i = 0; i < len; ++i) {
            QChar ch = script[i];
            // Single‑line comment '--'
            if (!inString && ch == '-' && i + 1 < len && script[i + 1] == '-') {
                i += 2;
                while (i < len && script[i] != '\n' && script[i] != '\r') ++i;
                continue;
            }
            // Block comment '/* ... */'
            if (!inString && ch == '/' && i + 1 < len && script[i + 1] == '*') {
                i += 2;
                while (i + 1 < len && !(script[i] == '*' && script[i + 1] == '/')) ++i;
                i += 1; // skip closing */
                continue;
            }
            // Handle string literals delimited by single quotes, with SQLite escaping ''
            if (ch == '\'') {
                if (inString) {
                    // Peek ahead for escaped ''
                    if (i + 1 < len && script[i + 1] == '\'') {
                        // keep both quotes as part of the string
                        stmt.append(ch);
                        ++i; // consume the second quote
                        stmt.append('\'');
                        continue;
                    } else {
                        inString = false;
                    }
                } else {
                    inString = true;
                }
            }
            stmt.append(ch);
            // Statement terminator when not inside a string literal
            if (!inString && ch == ';') {
                QString cur = stmt.trimmed();
                if (cur.endsWith(';')) cur.chop(1);
                cur = cur.trimmed();
                if (!cur.isEmpty()) {
                    if (!query.exec(cur)) {
                        qCritical() << "SQL exec error:" << cur << query.lastError().text();
                        return false;
                    }
                }
                stmt.clear();
            }
        }
        // Execute any trailing statement that may not end with ';'
        QString trailing = stmt.trimmed();
        if (!trailing.isEmpty()) {
            if (!query.exec(trailing)) {
                qCritical() << "SQL exec error (trailing):" << trailing << query.lastError().text();
                return false;
            }
        }
        return true;
    };

    QCoreApplication app(argc, argv);
    QString baseDir = QCoreApplication::applicationDirPath(); // location of executable
    // Assume scripts are copied to the same directory as executable
    QString dbPath = baseDir + "/test_word_sprint.db";
    // Remove existing DB if any
    QFile::remove(dbPath);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qCritical() << "Failed to open DB:" << db.lastError().text();
        return 1;
    }
    // Execute schema and seed scripts
    QFile schemaFile(baseDir + "/schema.sql");
    QFile seedFile(baseDir + "/seed.sql");
    // Fallback to source directory if files are not found next to the executable
    if (!schemaFile.exists() || !seedFile.exists()) {
        QString fallbackBase = QCoreApplication::applicationDirPath() + "/../backend/database";
        if (!schemaFile.exists()) schemaFile.setFileName(fallbackBase + "/schema.sql");
        if (!seedFile.exists()) seedFile.setFileName(fallbackBase + "/seed.sql");
    }
    if (!schemaFile.open(QIODevice::ReadOnly) || !seedFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open schema or seed files";
        return 1;
    }
    QString schemaSql = QString::fromUtf8(schemaFile.readAll());
    QString seedSql = QString::fromUtf8(seedFile.readAll());
    QSqlQuery query(db);
    if (!executeSqlScript(db, schemaSql)) {
        qCritical() << "Schema exec error";
        return 1;
    }
    if (!executeSqlScript(db, seedSql)) {
        qCritical() << "Seed exec error";
        return 1;
    }
    // Verify CEFR levels
    QSet<QString> levels;
    if (!query.exec("SELECT DISTINCT cefr_level FROM words")) {
        qCritical() << "Level query error:" << query.lastError().text();
        return 1;
    }
    while (query.next()) {
        levels.insert(query.value(0).toString());
    }
    QSet<QString> expected = {"A1","A2","B1","B2","C1","C2"};
    if (levels != expected) {
        qCritical() << "FAIL: CEFR levels mismatch" << levels;
        return 1;
    }
    // Verify topics count
    if (!query.exec("SELECT COUNT(*) FROM topics")) {
        qCritical() << "Topic count error:" << query.lastError().text();
        return 1;
    }
    query.next();
    int topicCount = query.value(0).toInt();
    if (topicCount < 6) {
        qCritical() << "FAIL: insufficient topics" << topicCount;
        return 1;
    }
    // Verify at least 10 words per level
    bool ok = true;
    for (const QString &lvl : expected) {
        query.prepare("SELECT COUNT(*) FROM words WHERE cefr_level = ?");
        query.addBindValue(lvl);
        if (!query.exec() || !query.next()) {
            qCritical() << "Count query failed for" << lvl;
            ok = false;
            continue;
        }
        int cnt = query.value(0).toInt();
        if (cnt < 10) {
            qCritical() << "FAIL: level" << lvl << "has only" << cnt << "words";
            ok = false;
        }
    }
    if (!ok) return 1;
    qInfo() << "PASS: All checks passed";
    return 0;
}
