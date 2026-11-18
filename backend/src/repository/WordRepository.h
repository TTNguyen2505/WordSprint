// WordRepository.h - SQLite DAO / Repository mapping rows to pure C++ models
#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <vector>
#include "../model/Topic.h"
#include "../model/Word.h"

class WordRepository : public QObject {
    Q_OBJECT

public:
    explicit WordRepository(QObject* parent = nullptr);
    ~WordRepository() override;

    bool initialize(const QString& customDbPath = QString());
    bool isInitialized() const { return m_initialized; }
    QString databasePath() const { return m_dbPath; }

    // Query & Map SQLite rows -> Pure C++ Topic / Word objects
    std::vector<Topic> getAllTopics() const;
    Topic getTopicById(int topicId) const;
    std::vector<Word> getAllWords() const;
    std::vector<Word> getWordsByTopic(int topicId) const;
    std::vector<Word> getWordsByCefrLevel(const std::string& cefrLevel) const;
    int countWords() const;

    // CRUD operations
    bool addWord(Word& word);
    bool updateWord(int id, const std::string& newText);
    bool removeWord(int id);
    bool replaceAllWords(const std::vector<Word>& newWords);
    bool resetToSeedData();

private:
    QSqlDatabase database() const;
    QString resolveScriptPath(const QString& fileName) const;
    bool executeSqlFile(const QString& filePath);
    bool executeSqlScript(const QString& script);
    int resolveDefaultTopicId() const;

    QString m_connectionName;
    QString m_dbPath;
    bool m_initialized = false;
};
