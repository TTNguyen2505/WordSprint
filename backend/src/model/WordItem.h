#pragma once

#include <QJsonObject>
#include <QString>

struct WordItem {
    int id{0};
    QString word;
    QString phonetic;
    QString partOfSpeech;
    QString meaningVi;
    QString definitionEn;
    QString exampleSentence;
    QString exampleTranslation;
    QString hintImage;
    QString audioPath;
    int difficultyLevel{1}; // 1: A1, 2: A2, 3: B1, 4: B2
    int topicId{1};

    bool isValid() const {
        return id > 0 && !word.trimmed().isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj[QStringLiteral("id")] = id;
        obj[QStringLiteral("word")] = word;
        obj[QStringLiteral("phonetic")] = phonetic;
        obj[QStringLiteral("partOfSpeech")] = partOfSpeech;
        obj[QStringLiteral("meaningVi")] = meaningVi;
        obj[QStringLiteral("definitionEn")] = definitionEn;
        obj[QStringLiteral("exampleSentence")] = exampleSentence;
        obj[QStringLiteral("exampleTranslation")] = exampleTranslation;
        obj[QStringLiteral("hintImage")] = hintImage;
        obj[QStringLiteral("audioPath")] = audioPath;
        obj[QStringLiteral("difficultyLevel")] = difficultyLevel;
        obj[QStringLiteral("topicId")] = topicId;
        return obj;
    }

    static WordItem fromJson(const QJsonObject &json) {
        WordItem item;
        item.id = json.value(QStringLiteral("id")).toInt();
        item.word = json.value(QStringLiteral("word")).toString();
        item.phonetic = json.value(QStringLiteral("phonetic")).toString();
        item.partOfSpeech = json.value(QStringLiteral("partOfSpeech")).toString();
        item.meaningVi = json.value(QStringLiteral("meaningVi")).toString();
        item.definitionEn = json.value(QStringLiteral("definitionEn")).toString();
        item.exampleSentence = json.value(QStringLiteral("exampleSentence")).toString();
        item.exampleTranslation = json.value(QStringLiteral("exampleTranslation")).toString();
        item.hintImage = json.value(QStringLiteral("hintImage")).toString();
        item.audioPath = json.value(QStringLiteral("audioPath")).toString();
        item.difficultyLevel = json.value(QStringLiteral("difficultyLevel")).toInt(1);
        item.topicId = json.value(QStringLiteral("topicId")).toInt(1);
        return item;
    }
};
