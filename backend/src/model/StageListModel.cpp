#include "StageListModel.h"

StageListModel::StageListModel(QObject *parent)
    : QAbstractListModel(parent) {
    loadMockData();
}

int StageListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_displayItems.size();
}

QVariant StageListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_displayItems.size()) {
        return QVariant();
    }

    const auto &item = m_displayItems.at(index.row());

    switch (role) {
    case IdRole:
        return item.id;
    case TopicIdRole:
        return item.topicId;
    case CefrLevelRole:
        return item.cefrLevel;
    case TitleRole:
    case Qt::DisplayRole:
        return item.title;
    case TimeLimitRole:
        return item.timeLimit;
    case TotalQuestionsRole:
        return item.totalQuestions;
    case IsUnlockedRole:
        return item.isUnlocked;
    case IsCompletedRole:
        return item.isCompleted;
    case BestScoreRole:
        return item.bestScore;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> StageListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TopicIdRole] = "topicId";
    roles[CefrLevelRole] = "cefrLevel";
    roles[TitleRole] = "title";
    roles[TimeLimitRole] = "timeLimit";
    roles[TotalQuestionsRole] = "totalQuestions";
    roles[IsUnlockedRole] = "isUnlocked";
    roles[IsCompletedRole] = "isCompleted";
    roles[BestScoreRole] = "bestScore";
    return roles;
}

void StageListModel::loadMockData() {
    QVector<Stage> mockList = {
        {1, 1, QStringLiteral("A1"), QStringLiteral("Stage 1: OOP Fundamentals"), 60, 5, true, true, 480},
        {2, 1, QStringLiteral("A2"), QStringLiteral("Stage 2: Encapsulation & Inheritance"), 60, 5, true, false, 0},
        {3, 1, QStringLiteral("B1"), QStringLiteral("Stage 3: Polymorphism & Abstraction"), 45, 5, false, false, 0},
        {4, 1, QStringLiteral("B2"), QStringLiteral("Stage 4: Advanced Design Patterns"), 45, 5, false, false, 0},
        {5, 2, QStringLiteral("A1"), QStringLiteral("Stage 1: Basic Data Types"), 60, 5, true, false, 0},
        {6, 2, QStringLiteral("A2"), QStringLiteral("Stage 2: Linear Data Structures"), 60, 5, false, false, 0}
    };

    m_allItems = mockList;
    updateDisplayItems(mockList);
}

QVariantMap StageListModel::get(int index) const {
    if (index < 0 || index >= m_displayItems.size()) {
        return QVariantMap();
    }
    const auto &item = m_displayItems.at(index);
    QVariantMap map;
    map[QStringLiteral("id")] = item.id;
    map[QStringLiteral("topicId")] = item.topicId;
    map[QStringLiteral("cefrLevel")] = item.cefrLevel;
    map[QStringLiteral("title")] = item.title;
    map[QStringLiteral("timeLimit")] = item.timeLimit;
    map[QStringLiteral("totalQuestions")] = item.totalQuestions;
    map[QStringLiteral("isUnlocked")] = item.isUnlocked;
    map[QStringLiteral("isCompleted")] = item.isCompleted;
    map[QStringLiteral("bestScore")] = item.bestScore;
    return map;
}

void StageListModel::filterByTopic(int topicId) {
    if (topicId <= 0) {
        resetFilter();
        return;
    }

    QVector<Stage> filtered;
    for (const auto &item : m_allItems) {
        if (item.topicId == topicId) {
            filtered.append(item);
        }
    }
    updateDisplayItems(filtered);
}

void StageListModel::unlockStage(int stageId) {
    for (int i = 0; i < m_allItems.size(); ++i) {
        if (m_allItems[i].id == stageId) {
            m_allItems[i].isUnlocked = true;
            break;
        }
    }
    for (int i = 0; i < m_displayItems.size(); ++i) {
        if (m_displayItems[i].id == stageId) {
            m_displayItems[i].isUnlocked = true;
            emit dataChanged(index(i), index(i), {IsUnlockedRole});
            break;
        }
    }
}

void StageListModel::updateScore(int stageId, int score) {
    for (int i = 0; i < m_allItems.size(); ++i) {
        if (m_allItems[i].id == stageId) {
            if (score > m_allItems[i].bestScore) {
                m_allItems[i].bestScore = score;
            }
            m_allItems[i].isCompleted = true;
            break;
        }
    }
    for (int i = 0; i < m_displayItems.size(); ++i) {
        if (m_displayItems[i].id == stageId) {
            if (score > m_displayItems[i].bestScore) {
                m_displayItems[i].bestScore = score;
            }
            m_displayItems[i].isCompleted = true;
            emit dataChanged(index(i), index(i), {BestScoreRole, IsCompletedRole});
            break;
        }
    }
}

void StageListModel::resetFilter() {
    updateDisplayItems(m_allItems);
}

void StageListModel::updateDisplayItems(const QVector<Stage> &newItems) {
    beginResetModel();
    m_displayItems = newItems;
    endResetModel();
    emit countChanged();
}
