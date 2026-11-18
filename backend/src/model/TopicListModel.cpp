#include "TopicListModel.h"

TopicListModel::TopicListModel(QObject *parent)
    : QAbstractListModel(parent) {
    loadMockData();
}

int TopicListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant TopicListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto &item = m_items.at(index.row());

    switch (role) {
    case IdRole:
        return item.id;
    case NameRole:
    case Qt::DisplayRole:
        return item.name;
    case DescriptionRole:
        return item.description;
    case IconNameRole:
        return item.iconName;
    case WordCountRole:
        return item.wordCount;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TopicListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[DescriptionRole] = "description";
    roles[IconNameRole] = "iconName";
    roles[WordCountRole] = "wordCount";
    return roles;
}

void TopicListModel::loadMockData() {
    beginResetModel();
    m_items = {
        {1, QStringLiteral("OOP & Software Architecture"),
         QStringLiteral("Core concepts of object-oriented design and design patterns."),
         QStringLiteral("code"), 25},
        {2, QStringLiteral("Algorithms & Data Structures"),
         QStringLiteral("Fundamental algorithms, trees, graphs, and complexity."),
         QStringLiteral("device_hub"), 30},
        {3, QStringLiteral("Database & SQL"),
         QStringLiteral("Relational schema design, indexes, transactions, and SQLite."),
         QStringLiteral("storage"), 20},
        {4, QStringLiteral("Network & Web Protocols"),
         QStringLiteral("HTTP, WebSockets, client-server architectures, and APIs."),
         QStringLiteral("public"), 18}
    };
    endResetModel();
    emit countChanged();
}

QVariantMap TopicListModel::get(int index) const {
    if (index < 0 || index >= m_items.size()) {
        return QVariantMap();
    }
    const auto &item = m_items.at(index);
    QVariantMap map;
    map[QStringLiteral("id")] = item.id;
    map[QStringLiteral("name")] = item.name;
    map[QStringLiteral("description")] = item.description;
    map[QStringLiteral("iconName")] = item.iconName;
    map[QStringLiteral("wordCount")] = item.wordCount;
    return map;
}
