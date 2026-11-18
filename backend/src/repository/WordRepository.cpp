// WordRepository.cpp - SQLite DAO / Repository implementation
#include "WordRepository.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

namespace {
Word mapRowToWord(const QSqlQuery& query) {
    return Word(
        query.value(0).toInt(),
        query.value(1).toInt(),
        query.value(2).toString().toStdString(),
        query.value(3).toString().toStdString(),
        query.value(4).toString().toStdString(),
        query.value(5).toString().toStdString(),
        query.value(6).toString().toStdString(),
        query.value(7).toString().toStdString(),
        query.value(8).toString().toStdString());
}
}  // namespace

WordRepository::WordRepository(QObject* parent)
    : QObject(parent),
      m_connectionName(
          QStringLiteral("WordSprintRepo_%1").arg(reinterpret_cast<quintptr>(this), 0, 16)) {}

WordRepository::~WordRepository() {
    if (QSqlDatabase::contains(m_connectionName)) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
            if (db.isOpen()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

QSqlDatabase WordRepository::database() const {
    return QSqlDatabase::database(m_connectionName);
}

bool WordRepository::initialize(const QString& customDbPath) {
    if (m_initialized) {
        return true;
    }

    if (!customDbPath.isEmpty()) {
        m_dbPath = customDbPath;
        QDir().mkpath(QFileInfo(m_dbPath).absolutePath());
    } else {
        QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (appDataDir.isEmpty()) {
            appDataDir = QCoreApplication::applicationDirPath();
        }
        QDir().mkpath(appDataDir);
        m_dbPath = appDataDir + QStringLiteral("/wordsprint.db");
    }

    QSqlDatabase db = QSqlDatabase::contains(m_connectionName)
                          ? QSqlDatabase::database(m_connectionName)
                          : QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(m_dbPath);

    if (!db.open()) {
        qCritical() << "[WordRepository] Failed to open SQLite database:" << db.lastError().text();
        return false;
    }

    QSqlQuery pragmaQuery(db);
    pragmaQuery.exec(QStringLiteral("PRAGMA foreign_keys = ON;"));

    const QString schemaPath = resolveScriptPath(QStringLiteral("schema.sql"));
    if (schemaPath.isEmpty() || !executeSqlFile(schemaPath)) {
        qCritical() << "[WordRepository] Failed to execute schema.sql from:" << schemaPath;
        return false;
    }

    m_initialized = true;

    if (countWords() == 0) {
        if (!resetToSeedData()) {
            qWarning() << "[WordRepository] Seed data could not be loaded automatically.";
        }
    }

    return true;
}

QString WordRepository::resolveScriptPath(const QString& fileName) const {
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        appDir + QLatin1Char('/') + fileName,
        appDir + QStringLiteral("/database/") + fileName,
        appDir + QStringLiteral("/../backend/database/") + fileName,
        appDir + QStringLiteral("/../../backend/database/") + fileName,
        QStringLiteral("backend/database/") + fileName};

    for (const QString& path : candidates) {
        if (QFileInfo::exists(path)) {
            return QFileInfo(path).canonicalFilePath();
        }
    }
    return QString();
}

bool WordRepository::executeSqlFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "[WordRepository] Cannot open SQL file:" << filePath;
        return false;
    }
    const QString script = QString::fromUtf8(file.readAll());
    return executeSqlScript(script);
}

bool WordRepository::executeSqlScript(const QString& script) {
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return false;
    }

    QSqlQuery query(db);
    QString stmt;
    bool inString = false;
    const int len = script.length();

    for (int i = 0; i < len; ++i) {
        const QChar ch = script[i];

        if (!inString && ch == QLatin1Char('-') && i + 1 < len &&
            script[i + 1] == QLatin1Char('-')) {
            i += 2;
            while (i < len && script[i] != QLatin1Char('\n') && script[i] != QLatin1Char('\r')) {
                ++i;
            }
            continue;
        }

        if (!inString && ch == QLatin1Char('/') && i + 1 < len &&
            script[i + 1] == QLatin1Char('*')) {
            i += 2;
            while (i + 1 < len &&
                   !(script[i] == QLatin1Char('*') && script[i + 1] == QLatin1Char('/'))) {
                ++i;
            }
            i += 1;
            continue;
        }

        if (ch == QLatin1Char('\'')) {
            if (inString) {
                if (i + 1 < len && script[i + 1] == QLatin1Char('\'')) {
                    stmt.append(ch);
                    ++i;
                    stmt.append(QLatin1Char('\''));
                    continue;
                }
                inString = false;
            } else {
                inString = true;
            }
        }

        stmt.append(ch);

        if (!inString && ch == QLatin1Char(';')) {
            QString cur = stmt.trimmed();
            if (cur.endsWith(QLatin1Char(';'))) {
                cur.chop(1);
            }
            cur = cur.trimmed();
            if (!cur.isEmpty()) {
                if (!query.exec(cur)) {
                    qCritical() << "[WordRepository] SQL error:" << cur << query.lastError().text();
                    return false;
                }
            }
            stmt.clear();
        }
    }

    const QString trailing = stmt.trimmed();
    if (!trailing.isEmpty()) {
        if (!query.exec(trailing)) {
            qCritical() << "[WordRepository] SQL trailing error:" << trailing
                        << query.lastError().text();
            return false;
        }
    }

    return true;
}

int WordRepository::resolveDefaultTopicId() const {
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return 1;
    }

    QSqlQuery query(db);
    if (query.exec(QStringLiteral("SELECT id FROM topics ORDER BY id ASC LIMIT 1")) &&
        query.next()) {
        return query.value(0).toInt();
    }

    if (query.exec(QStringLiteral("INSERT OR IGNORE INTO topics (name) VALUES ('Daily Life')")) &&
        query.exec(QStringLiteral("SELECT id FROM topics WHERE name = 'Daily Life'")) &&
        query.next()) {
        return query.value(0).toInt();
    }

    return 1;
}

std::vector<Topic> WordRepository::getAllTopics() const {
    std::vector<Topic> topics;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return topics;
    }

    QSqlQuery query(db);
    if (query.exec(QStringLiteral("SELECT id, name FROM topics ORDER BY id ASC"))) {
        while (query.next()) {
            topics.emplace_back(query.value(0).toInt(), query.value(1).toString().toStdString());
        }
    }
    return topics;
}

Topic WordRepository::getTopicById(int topicId) const {
    QSqlDatabase db = database();
    if (!db.isOpen() || topicId <= 0) {
        return Topic{};
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT id, name FROM topics WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), topicId);
    if (query.exec() && query.next()) {
        return Topic(query.value(0).toInt(), query.value(1).toString().toStdString());
    }
    return Topic{};
}

int WordRepository::countWords() const {
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return 0;
    }

    QSqlQuery query(db);
    if (query.exec(QStringLiteral("SELECT COUNT(*) FROM words")) && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

std::vector<Word> WordRepository::getAllWords() const {
    std::vector<Word> result;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return result;
    }

    QSqlQuery query(db);
    const QString sql = QStringLiteral(
        "SELECT id, topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level "
        "FROM words ORDER BY id ASC");

    if (!query.exec(sql)) {
        qCritical() << "[WordRepository] getAllWords failed:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        result.push_back(mapRowToWord(query));
    }

    return result;
}

std::vector<Word> WordRepository::getWordsByTopic(int topicId) const {
    std::vector<Word> result;
    QSqlDatabase db = database();
    if (!db.isOpen() || topicId <= 0) {
        return result;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT id, topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level "
        "FROM words WHERE topic_id = :topic_id ORDER BY id ASC"));
    query.bindValue(QStringLiteral(":topic_id"), topicId);

    if (!query.exec()) {
        qCritical() << "[WordRepository] getWordsByTopic failed:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        result.push_back(mapRowToWord(query));
    }

    return result;
}

std::vector<Word> WordRepository::getWordsByCefrLevel(const std::string& cefrLevel) const {
    std::vector<Word> result;
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return result;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT id, topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level "
        "FROM words WHERE cefr_level = :level ORDER BY id ASC"));
    query.bindValue(QStringLiteral(":level"), QString::fromStdString(cefrLevel));

    if (!query.exec()) {
        qCritical() << "[WordRepository] getWordsByCefrLevel failed:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        result.push_back(mapRowToWord(query));
    }

    return result;
}

bool WordRepository::addWord(Word& word) {
    QSqlDatabase db = database();
    if (!db.isOpen() || word.text().empty()) {
        return false;
    }

    const int topicId = (word.topicId() > 0) ? word.topicId() : resolveDefaultTopicId();
    const QString cefr = word.cefrLevel().empty()
                             ? QStringLiteral("A1")
                             : QString::fromStdString(word.cefrLevel());

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO words (topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level) "
        "VALUES (:topic_id, :word, :phonetic, :pos, :meaning_en, :meaning_vi, :example, :cefr_level)"));
    query.bindValue(QStringLiteral(":topic_id"), topicId);
    query.bindValue(QStringLiteral(":word"), QString::fromStdString(word.text()));
    query.bindValue(QStringLiteral(":phonetic"), QString::fromStdString(word.phonetic()));
    query.bindValue(QStringLiteral(":pos"), QString::fromStdString(word.pos()));
    query.bindValue(QStringLiteral(":meaning_en"), QString::fromStdString(word.meaningEn()));
    query.bindValue(QStringLiteral(":meaning_vi"), QString::fromStdString(word.meaningVi()));
    query.bindValue(QStringLiteral(":example"), QString::fromStdString(word.example()));
    query.bindValue(QStringLiteral(":cefr_level"), cefr);

    if (!query.exec()) {
        qCritical() << "[WordRepository] addWord failed:" << query.lastError().text();
        return false;
    }

    word.setId(query.lastInsertId().toInt());
    word.setTopicId(topicId);
    return true;
}

bool WordRepository::updateWord(int id, const std::string& newText) {
    QSqlDatabase db = database();
    if (!db.isOpen() || id <= 0 || newText.empty()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE words SET word = :word WHERE id = :id"));
    query.bindValue(QStringLiteral(":word"), QString::fromStdString(newText));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        qCritical() << "[WordRepository] updateWord failed:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool WordRepository::removeWord(int id) {
    QSqlDatabase db = database();
    if (!db.isOpen() || id <= 0) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM words WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        qCritical() << "[WordRepository] removeWord failed:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool WordRepository::replaceAllWords(const std::vector<Word>& newWords) {
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return false;
    }

    if (!db.transaction()) {
        return false;
    }

    QSqlQuery clearQuery(db);
    if (!clearQuery.exec(QStringLiteral("DELETE FROM words"))) {
        db.rollback();
        return false;
    }

    for (const Word& item : newWords) {
        Word copy = item;
        if (!addWord(copy)) {
            db.rollback();
            return false;
        }
    }

    return db.commit();
}

bool WordRepository::resetToSeedData() {
    QSqlDatabase db = database();
    if (!db.isOpen()) {
        return false;
    }

    const QString seedPath = resolveScriptPath(QStringLiteral("seed.sql"));
    if (seedPath.isEmpty()) {
        qCritical() << "[WordRepository] seed.sql not found.";
        return false;
    }

    if (!db.transaction()) {
        return false;
    }

    QSqlQuery cleanup(db);
    if (!cleanup.exec(QStringLiteral("DELETE FROM words")) ||
        !cleanup.exec(QStringLiteral("DELETE FROM topics"))) {
        db.rollback();
        return false;
    }

    if (!executeSqlFile(seedPath)) {
        db.rollback();
        return false;
    }

    return db.commit();
}
