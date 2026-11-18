#pragma once

#include <QJsonObject>
#include <QString>

struct Topic {
    int id{0};
    QString name;
    QString description;
    QString iconName;
    int wordCount{0};

    bool isValid() const {
        return id > 0 && !name.trimmed().isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj[QStringLiteral("id")] = id;
        obj[QStringLiteral("name")] = name;
        obj[QStringLiteral("description")] = description;
        obj[QStringLiteral("iconName")] = iconName;
        obj[QStringLiteral("wordCount")] = wordCount;
        return obj;
    }

    static Topic fromJson(const QJsonObject &json) {
        Topic item;
        item.id = json.value(QStringLiteral("id")).toInt();
        item.name = json.value(QStringLiteral("name")).toString();
        item.description = json.value(QStringLiteral("description")).toString();
        item.iconName = json.value(QStringLiteral("iconName")).toString();
        item.wordCount = json.value(QStringLiteral("wordCount")).toInt();
        return item;
    }
};
