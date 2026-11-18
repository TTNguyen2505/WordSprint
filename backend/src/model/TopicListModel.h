#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "Topic.h"

class TopicListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum TopicRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        IconNameRole,
        WordCountRole
    };
    Q_ENUM(TopicRoles)

    explicit TopicListModel(QObject *parent = nullptr);
    ~TopicListModel() override = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void loadMockData();
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE int count() const { return rowCount(); }

    const QVector<Topic>& items() const { return m_items; }

signals:
    void countChanged();

private:
    QVector<Topic> m_items;
};
