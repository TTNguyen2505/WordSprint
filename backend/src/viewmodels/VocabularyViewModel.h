#pragma once

#include "../repositories/IVocabularyRepository.h"

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

#include <vector>

class VocabularyViewModel final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList topics READ topics NOTIFY topicsChanged)
    Q_PROPERTY(QVariantList vocabularies READ vocabularies NOTIFY vocabulariesChanged)

public:
    explicit VocabularyViewModel(IVocabularyRepository& repository, QObject* parent = nullptr);

    QVariantList topics() const;
    QVariantList vocabularies() const;

    Q_INVOKABLE int loadTopics();
    Q_INVOKABLE int loadVocabulariesByTopic(int topicId);
    Q_INVOKABLE int loadVocabulariesByLevel(const QString& cefrLevel);
    Q_INVOKABLE int loadRandomVocabularies(
        int limit,
        int topicId = -1,
        const QString& cefrLevel = QString());
    Q_INVOKABLE QVariantMap getProgressByVocabId(int vocabId);
    Q_INVOKABLE bool updateLearningProgress(int vocabId, bool isCorrect);

signals:
    void topicsChanged();
    void vocabulariesChanged();
    void learningProgressChanged(int vocabId);

private:
    void setVocabularies(const std::vector<Vocabulary>& vocabularies);

    IVocabularyRepository& m_repository;
    QVariantList m_topics;
    QVariantList m_vocabularies;
};
