// VocabularyViewModel.h
// ViewModel exposing SQLite data to QML. Keeps original Q_PROPERTY, Q_INVOKABLE and signals.
// Integrates DatabaseManager and VocabularyRepository.

#pragma once

#include <QObject>
#include <QVariantList>
#include <memory>

#include "IVocabularyRepository.h"
#include "VocabularyRepository.h"

class VocabularyViewModel : public QObject {
    Q_OBJECT
public:
    explicit VocabularyViewModel(std::shared_ptr<DatabaseManager> dbMgr, QObject *parent = nullptr);
    ~VocabularyViewModel() override = default;

    // Expose lists to QML
    Q_PROPERTY(QVariantList levels READ levels NOTIFY levelsChanged)
    Q_PROPERTY(QVariantList topics READ topics NOTIFY topicsChanged)
    Q_PROPERTY(QVariantList vocabularies READ vocabularies NOTIFY vocabulariesChanged)

    // Current filters (optional, can be bound from QML)
    Q_PROPERTY(int selectedLevelId READ selectedLevelId WRITE setSelectedLevelId NOTIFY selectedLevelIdChanged)
    Q_PROPERTY(int selectedTopicId READ selectedTopicId WRITE setSelectedTopicId NOTIFY selectedTopicIdChanged)
    Q_PROPERTY(QString searchKeyword READ searchKeyword WRITE setSearchKeyword NOTIFY searchKeywordChanged)

    // --- Accessors ----------------------------------------------------------
    QVariantList levels() const { return m_levels; }
    QVariantList topics() const { return m_topics; }
    QVariantList vocabularies() const { return m_vocabularies; }

    int selectedLevelId() const { return m_selectedLevelId; }
    void setSelectedLevelId(int id);

    int selectedTopicId() const { return m_selectedTopicId; }
    void setSelectedTopicId(int id);

    QString searchKeyword() const { return m_searchKeyword; }
    void setSearchKeyword(const QString &kw);

    // --- Invokable methods -------------------------------------------------
    Q_INVOKABLE void loadLevels();
    Q_INVOKABLE void loadTopics();
    Q_INVOKABLE void loadVocabularies();
    Q_INVOKABLE void filterByLevel(int levelId);
    Q_INVOKABLE void filterByTopic(int topicId);
    Q_INVOKABLE void search(const QString &keyword);

signals:
    void levelsChanged();
    void topicsChanged();
    void vocabulariesChanged();
    void selectedLevelIdChanged();
    void selectedTopicIdChanged();
    void searchKeywordChanged();

private:
    std::shared_ptr<DatabaseManager> m_dbMgr;
    std::unique_ptr<VocabularyRepository> m_repo;
    QVariantList m_levels;
    QVariantList m_topics;
    QVariantList m_vocabularies;
    int m_selectedLevelId = -1;
    int m_selectedTopicId = -1;
    QString m_searchKeyword;

    // Helper to convert native structs to QVariantMap for QML.
    QVariantMap levelToMap(const Level &lvl) const;
    QVariantMap topicToMap(const Topic &top) const;
    QVariantMap vocabToMap(const Vocabulary &voc) const;
};
