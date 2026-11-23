#pragma once

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

#include "repositories/IVocabularyRepository.h"

/**
 * @brief BridgeController
 *
 * QObject-derived controller exposed to the React frontend through Qt WebChannel.
 * Implements the communication contract specified in Chapter 2.4 and Chapter 2.5:
 *  - startStage: loads vocabulary through the repository and returns stage data to React.
 *  - completeStage: updates in-memory stage progress and returns a completion summary.
 *  - processAnswer: evaluates an answer and records vocabulary progress through the repository.
 *  - processMessage: universal dispatcher for JSON actions from the web frontend.
 */
class BridgeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)

public:
    explicit BridgeController(IVocabularyRepository* repository = nullptr, QObject *parent = nullptr);
    ~BridgeController() override = default;

    bool isReady() const { return m_isReady; }

public slots:
    /**
     * @brief Start a game stage.
     * @param requestJson JSON string, e.g. { "stageId": 1 }
     * @return JSON string response containing stage parameters and word items list.
     */
    Q_INVOKABLE QString startStage(const QString &requestJson);

    Q_INVOKABLE QString getAllTopics();
    Q_INVOKABLE QString getStagesByTopicId(int topicId);

    /**
     * @brief Complete a stage and update in-memory progress.
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
    IVocabularyRepository* m_repository = nullptr;

    struct StageProgress {
        int recordId = 0;
        int highScore = 0;
        int bestTime = 0;
    };

    QHash<QString, StageProgress> m_stageProgress;
    QHash<int, int> m_unlockedStagesByUser;
    int m_nextRecordId = 1;

    // Safe JSON conversion helpers
    QJsonObject parseJson(const QString &jsonStr) const;
    QString buildResponse(const QVariantMap &obj) const;
    QString buildErrorResponse(const QString &errorMessage) const;
};
