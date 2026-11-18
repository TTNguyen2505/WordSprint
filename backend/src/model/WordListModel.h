#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "WordItem.h"

class WordListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum WordRoles {
        IdRole = Qt::UserRole + 1,
        WordRole,
        PhoneticRole,
        PartOfSpeechRole,
        MeaningViRole,
        DefinitionEnRole,
        ExampleSentenceRole,
        ExampleTranslationRole,
        HintImageRole,
        AudioPathRole,
        DifficultyLevelRole,
        TopicIdRole
    };
    Q_ENUM(WordRoles)

    explicit WordListModel(QObject *parent = nullptr);
    ~WordListModel() override = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void loadMockData();
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE void addWord(const QVariantMap &item);
    Q_INVOKABLE void filterByTopic(int topicId);
    Q_INVOKABLE void search(const QString &keyword);
    Q_INVOKABLE void resetFilter();
    Q_INVOKABLE void clear();
    Q_INVOKABLE int count() const { return rowCount(); }

    const QVector<WordItem>& items() const { return m_displayItems; }
    void setWords(const QVector<WordItem> &words);

signals:
    void countChanged();

private:
    QVector<WordItem> m_allItems;
    QVector<WordItem> m_displayItems;

    void updateDisplayItems(const QVector<WordItem> &newItems);
};
