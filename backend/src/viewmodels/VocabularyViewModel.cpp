#include "VocabularyViewModel.h"

#include <QVariant>

namespace {

QVariant optionalStringValue(const std::optional<std::string>& value) {
    if (!value.has_value()) {
        return {};
    }
    return QString::fromStdString(*value);
}

QVariantMap topicToVariantMap(const Topic& topic) {
    QVariantMap item;
    item.insert(QStringLiteral("topic_id"), topic.topic_id);
    item.insert(QStringLiteral("topic_name"), QString::fromStdString(topic.topic_name));
    item.insert(QStringLiteral("description"), optionalStringValue(topic.description));
    return item;
}

QVariantMap vocabularyToVariantMap(const Vocabulary& vocabulary) {
    QVariantMap item;
    item.insert(QStringLiteral("word_id"), vocabulary.word_id);
    item.insert(QStringLiteral("topic_id"), vocabulary.topic_id);
    item.insert(QStringLiteral("cefr_level"), QString::fromStdString(vocabulary.cefr_level));
    item.insert(QStringLiteral("target_word"), QString::fromStdString(vocabulary.target_word));
    item.insert(QStringLiteral("hint_text"), optionalStringValue(vocabulary.hint_text));
    item.insert(QStringLiteral("image_path"), optionalStringValue(vocabulary.image_path));
    item.insert(QStringLiteral("audio_path"), QString::fromStdString(vocabulary.audio_path));
    return item;
}

}  // namespace

VocabularyViewModel::VocabularyViewModel(IVocabularyRepository& repository, QObject* parent)
    : QObject(parent), m_repository(repository) {}

QVariantList VocabularyViewModel::topics() const {
    return m_topics;
}

QVariantList VocabularyViewModel::vocabularies() const {
    return m_vocabularies;
}

int VocabularyViewModel::loadTopics() {
    const std::vector<Topic> topics = m_repository.getAllTopics();
    m_topics.clear();
    for (const Topic& topic : topics) {
        m_topics.append(topicToVariantMap(topic));
    }
    emit topicsChanged();
    return static_cast<int>(m_topics.size());
}

int VocabularyViewModel::loadVocabulariesByTopic(int topicId) {
    setVocabularies(m_repository.getVocabulariesByTopic(topicId));
    return static_cast<int>(m_vocabularies.size());
}

int VocabularyViewModel::loadVocabulariesByLevel(const QString& cefrLevel) {
    setVocabularies(m_repository.getVocabulariesByLevel(cefrLevel.toStdString()));
    return static_cast<int>(m_vocabularies.size());
}

int VocabularyViewModel::loadRandomVocabularies(int limit, int topicId, const QString& cefrLevel) {
    setVocabularies(m_repository.getRandomVocabularies(limit, topicId, cefrLevel.toStdString()));
    return static_cast<int>(m_vocabularies.size());
}

QVariantMap VocabularyViewModel::getProgressByVocabId(int vocabId) {
    const std::optional<LearningProgress> progress = m_repository.getProgressByVocabId(vocabId);
    if (!progress.has_value()) {
        return {};
    }

    QVariantMap item;
    item.insert(QStringLiteral("vocabId"), progress->vocabId);
    item.insert(QStringLiteral("correctCount"), progress->correctCount);
    item.insert(QStringLiteral("wrongCount"), progress->wrongCount);
    item.insert(QStringLiteral("masteryLevel"), progress->masteryLevel);
    item.insert(QStringLiteral("lastReviewed"), QString::fromStdString(progress->lastReviewed));
    return item;
}

bool VocabularyViewModel::updateLearningProgress(int vocabId, bool isCorrect) {
    const bool updated = m_repository.updateLearningProgress(vocabId, isCorrect);
    if (updated) {
        emit learningProgressChanged(vocabId);
    }
    return updated;
}

void VocabularyViewModel::setVocabularies(const std::vector<Vocabulary>& vocabularies) {
    m_vocabularies.clear();
    for (const Vocabulary& vocabulary : vocabularies) {
        m_vocabularies.append(vocabularyToVariantMap(vocabulary));
    }
    emit vocabulariesChanged();
}
