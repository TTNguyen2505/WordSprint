#pragma once

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

#include <memory>

class RecordRepository;
class StageRepository;
class UserRepository;
namespace ReactFrontend_DomainLayer {
    class GameSession;
}

/**
 * @brief BridgeController
 *
 * QObject-derived controller exposed to the React frontend through Qt WebChannel.
 * Implements the communication contract specified in Chapter 2.4 and Chapter 2.5:
 *  - startStage: checks the user's stage unlock, loads vocabulary, and returns generated questions to React.
 *  - completeStage: persists a stage record and returns a completion summary.
 *  - processAnswer: evaluates an answer and returns the result to React.
 *  - processMessage: universal dispatcher for JSON actions from the web frontend.
 */
class BridgeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)

public:
    explicit BridgeController(StageRepository *stageRepository = nullptr, RecordRepository *recordRepository = nullptr,
                              UserRepository *userRepository = nullptr, QObject *parent = nullptr);
    ~BridgeController() override;

    bool isReady() const { return m_isReady; }

public slots:
    /**
     * @brief Start a game stage.
     * @param requestJson JSON string, e.g. { "stageId": 1, "userId": 1 }
     * @return JSON string response containing stage parameters and a complete generated question list.
     */
    Q_INVOKABLE QString startStage(const QString &requestJson);

    Q_INVOKABLE QString getAllTopics();
    Q_INVOKABLE QString getStagesByTopicId(int topicId);
    Q_INVOKABLE QString getProfiles(const QString &requestJson);
    Q_INVOKABLE QString createProfile(const QString &requestJson);
    Q_INVOKABLE QString updateAudio(const QString &requestJson);
    Q_INVOKABLE QString getLeaderboard(const QString &requestJson);

    /**
     * @brief Complete a stage and persist its record.
     * @param requestJson JSON string, e.g. { "userId": 1, "stageId": 1, "score": 90, "completionTime": 42 }
     * @return JSON string response with status, record, updated progress, and next stage info.
     */
    Q_INVOKABLE QString completeStage(const QString &requestJson);

    /**
     * @brief Process an answer submitted by the frontend.
     * @param requestJson JSON string matching Chapter 3.4/4.2.3 data contract.
     * @return JSON string response with evaluation status and score.
     */
    Q_INVOKABLE QString processAnswer(const QString &requestJson);

    /**
     * @brief Universal message processor for messages sent via window.qt.webChannel.
     * @param requestJson JSON payload with "action" and "payload" fields.
     * @return JSON string response.
     */
    Q_INVOKABLE QString processMessage(const QString &requestJson);

    /**
     * @brief Save user progress state in memory.
     */
    Q_INVOKABLE QString saveProgress(const QString &requestJson);

    /**
     * @brief Load user progress state.
     */
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
    std::unique_ptr<ReactFrontend_DomainLayer::GameSession> m_gameSession;
    QHash<int, int> m_unlockedStagesByUser;

    QJsonObject parseJson(const QString &jsonStr) const;
    QString buildResponse(const QVariantMap &obj) const;
    QString buildErrorResponse(const QString &errorMessage) const;
};
