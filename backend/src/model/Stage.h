#pragma once

#include <QJsonObject>
#include <QString>

struct Stage {
    int id{0};
    int topicId{1};
    QString cefrLevel{QStringLiteral("A1")};
    QString title;
    int timeLimit{60};       // seconds
    int totalQuestions{10};
    bool isUnlocked{false};
    bool isCompleted{false};
    int bestScore{0};

    bool isValid() const {
        return id > 0 && !title.trimmed().isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj[QStringLiteral("id")] = id;
        obj[QStringLiteral("topicId")] = topicId;
        obj[QStringLiteral("cefrLevel")] = cefrLevel;
        obj[QStringLiteral("title")] = title;
        obj[QStringLiteral("timeLimit")] = timeLimit;
        obj[QStringLiteral("totalQuestions")] = totalQuestions;
        obj[QStringLiteral("isUnlocked")] = isUnlocked;
        obj[QStringLiteral("isCompleted")] = isCompleted;
        obj[QStringLiteral("bestScore")] = bestScore;
        return obj;
    }

    static Stage fromJson(const QJsonObject &json) {
        Stage item;
        item.id = json.value(QStringLiteral("id")).toInt();
        item.topicId = json.value(QStringLiteral("topicId")).toInt(1);
        item.cefrLevel = json.value(QStringLiteral("cefrLevel")).toString(QStringLiteral("A1"));
        item.title = json.value(QStringLiteral("title")).toString();
        item.timeLimit = json.value(QStringLiteral("timeLimit")).toInt(60);
        item.totalQuestions = json.value(QStringLiteral("totalQuestions")).toInt(10);
        item.isUnlocked = json.value(QStringLiteral("isUnlocked")).toBool(false);
        item.isCompleted = json.value(QStringLiteral("isCompleted")).toBool(false);
        item.bestScore = json.value(QStringLiteral("bestScore")).toInt(0);
        return item;
    }
};
