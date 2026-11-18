// VocabularyViewModel.cpp
// Implementation of VocabularyViewModel – bridges SQLite repository to QML.

#include "VocabularyViewModel.h"
#include "DatabaseManager.h"
#include "VocabularyRepository.h"

#include <QDebug>
#include <QSqlQuery>

VocabularyViewModel::VocabularyViewModel(std::shared_ptr<DatabaseManager> dbMgr, QObject *parent)
    : QObject(parent), m_dbMgr(std::move(dbMgr)) {
    // Repository takes ownership of the shared DatabaseManager pointer.
    m_repo = std::make_unique<VocabularyRepository>(m_dbMgr);
    // Load initial lookup data.
    loadLevels();
    loadTopics();
    // Optionally load all vocabularies (unfiltered) if UI expects it on start.
    loadVocabularies();
}
    : QObject(parent), m_dbMgr(std::move(dbMgr)) {
    // Repository takes ownership of the shared DatabaseManager pointer.
    m_repo = std::make_unique<VocabularyRepository>(m_dbMgr);
}

QVariantMap VocabularyViewModel::levelToMap(const Level &lvl) const {
    QVariantMap map;
    map["id"] = lvl.id;
    map["code"] = QString::fromStdString(lvl.code);
    map["name"] = QString::fromStdString(lvl.name);
    map["description"] = QString::fromStdString(lvl.description);
    return map;
}

QVariantMap VocabularyViewModel::topicToMap(const Topic &top) const {
    QVariantMap map;
    map["id"] = top.id;
    map["name"] = QString::fromStdString(top.name);
    map["description"] = QString::fromStdString(top.description);
    return map;
}

QVariantMap VocabularyViewModel::vocabToMap(const Vocabulary &voc) const {
    QVariantMap map;
    map["id"] = voc.id;
    map["word"] = QString::fromStdString(voc.word);
    map["phonetic"] = QString::fromStdString(voc.phonetic);
    map["part_of_speech"] = QString::fromStdString(voc.part_of_speech);
    map["meaning_vi"] = QString::fromStdString(voc.meaning_vi);
    map["definition_en"] = QString::fromStdString(voc.definition_en);
    map["example_sentence"] = QString::fromStdString(voc.example_sentence);
    map["example_translation"] = QString::fromStdString(voc.example_translation);
    map["level_id"] = voc.level_id;
    map["topic_id"] = voc.topic_id;
    map["created_at"] = static_cast<qint64>(voc.created_at.time_since_epoch().count());
    return map;
}

void VocabularyViewModel::loadLevels() {
    m_levels.clear();
    const auto levels = m_repo->getAllLevels();
    for (const auto &lvl : levels) {
        m_levels.append(levelToMap(lvl));
    }
    emit levelsChanged();
}

void VocabularyViewModel::loadTopics() {
    m_topics.clear();
    const auto topics = m_repo->getAllTopics();
    for (const auto &top : topics) {
        m_topics.append(topicToMap(top));
    }
    emit topicsChanged();
}

void VocabularyViewModel::setSelectedLevelId(int id) {
    if (m_selectedLevelId != id) {
        m_selectedLevelId = id;
        emit selectedLevelIdChanged();
        filterByLevel(id);
    }
}

void VocabularyViewModel::setSelectedTopicId(int id) {
    if (m_selectedTopicId != id) {
        m_selectedTopicId = id;
        emit selectedTopicIdChanged();
        filterByTopic(id);
    }
}

void VocabularyViewModel::setSearchKeyword(const QString &kw) {
    if (m_searchKeyword != kw) {
        m_searchKeyword = kw;
        emit searchKeywordChanged();
        search(kw);
    }
}

void VocabularyViewModel::filterByLevel(int levelId) {
    if (!m_repo) return;
    m_vocabularies.clear();
    const auto list = m_repo->getVocabulariesByLevel(levelId);
    for (const auto &voc : list) {
        m_vocabularies.append(vocabToMap(voc));
    }
    emit vocabulariesChanged();
}

void VocabularyViewModel::filterByTopic(int topicId) {
    if (!m_repo) return;
    m_vocabularies.clear();
    const auto list = m_repo->getVocabulariesByTopic(topicId);
    for (const auto &voc : list) {
        m_vocabularies.append(vocabToMap(voc));
    }
    emit vocabulariesChanged();
}

void VocabularyViewModel::search(const QString &keyword) {
    if (!m_repo) return;
    m_vocabularies.clear();
    const auto list = m_repo->searchVocabularies(keyword.toStdString());
    for (const auto &voc : list) {
        m_vocabularies.append(vocabToMap(voc));
    }
    emit vocabulariesChanged();
}

void VocabularyViewModel::loadVocabularies() {
    // Load all vocabularies without filter – used on first app start.
    if (!m_repo) return;
    m_vocabularies.clear();
    const auto list = m_repo->getVocabulariesFilter("", {});
    for (const auto &voc : list) {
        m_vocabularies.append(vocabToMap(voc));
    }
    emit vocabulariesChanged();
}
