#pragma once

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class RecordRepository;
class StageRepository;
class UserRepository;

class BridgeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)

public:
    explicit BridgeController(StageRepository *stageRepository = nullptr, RecordRepository *recordRepository = nullptr,
                              UserRepository *userRepository = nullptr, QObject *parent = nullptr);
    ~BridgeController() override = default;

    bool isReady() const { return m_isReady; }

public slots:
    Q_INVOKABLE QString startStage(const QString &requestJson);

    Q_INVOKABLE QString getAllTopics();
    Q_INVOKABLE QString getStagesByTopicId(int topicId);
    Q_INVOKABLE QString getProfiles(const QString &requestJson);
    Q_INVOKABLE QString createProfile(const QString &requestJson);
    Q_INVOKABLE QString updateAudio(const QString &requestJson);
    Q_INVOKABLE QString getLeaderboard(const QString &requestJson);

    Q_INVOKABLE QString completeStage(const QString &requestJson);

    Q_INVOKABLE QString processAnswer(const QString &requestJson);

    Q_INVOKABLE QString processMessage(const QString &requestJson);

    Q_INVOKABLE QString saveProgress(const QString &requestJson);

    Q_INVOKABLE QString loadProgress(const QString &requestJson);

signals:
    void isReadyChanged(bool isReady);
    void stageStarted(int stageId);
    void stageCompleted(int stageId, int score);
    void jsonMessage(const QString &payload);

private:
    bool m_isReady = true;
    StageRepository *m_stageRepository = nullptr;
    RecordRepository *m_recordRepository = nullptr;
    UserRepository *m_userRepository = nullptr;
    QHash<int, int> m_unlockedStagesByUser;

    QJsonObject parseJson(const QString &jsonStr) const;
    QString buildResponse(const QVariantMap &obj) const;
    QString buildErrorResponse(const QString &errorMessage) const;
};
