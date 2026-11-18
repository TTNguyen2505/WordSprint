#pragma once

#include <QJsonObject>
#include <QString>

struct UserProfile {
    int id{0};
    QString username;
    int totalScore{0};
    int stagesCompleted{0};
    int currentStreak{0};

    bool isValid() const {
        return id > 0 && !username.trimmed().isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj[QStringLiteral("id")] = id;
        obj[QStringLiteral("username")] = username;
        obj[QStringLiteral("totalScore")] = totalScore;
        obj[QStringLiteral("stagesCompleted")] = stagesCompleted;
        obj[QStringLiteral("currentStreak")] = currentStreak;
        return obj;
    }

    static UserProfile fromJson(const QJsonObject &json) {
        UserProfile profile;
        profile.id = json.value(QStringLiteral("id")).toInt();
        profile.username = json.value(QStringLiteral("username")).toString();
        profile.totalScore = json.value(QStringLiteral("totalScore")).toInt(0);
        profile.stagesCompleted = json.value(QStringLiteral("stagesCompleted")).toInt(0);
        profile.currentStreak = json.value(QStringLiteral("currentStreak")).toInt(0);
        return profile;
    }
};
