#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "Stage.h"

class StageListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum StageRoles {
        IdRole = Qt::UserRole + 1,
        TopicIdRole,
        CefrLevelRole,
        TitleRole,
        TimeLimitRole,
        TotalQuestionsRole,
        IsUnlockedRole,
        IsCompletedRole,
        BestScoreRole
    };
    Q_ENUM(StageRoles)

    explicit StageListModel(QObject *parent = nullptr);
    ~StageListModel() override = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void loadMockData();
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE void filterByTopic(int topicId);
    Q_INVOKABLE void unlockStage(int stageId);
    Q_INVOKABLE void updateScore(int stageId, int score);
    Q_INVOKABLE void resetFilter();
    Q_INVOKABLE int count() const { return rowCount(); }

    const QVector<Stage>& items() const { return m_displayItems; }

signals:
    void countChanged();

private:
    QVector<Stage> m_allItems;
    QVector<Stage> m_displayItems;

    void updateDisplayItems(const QVector<Stage> &newItems);
};
