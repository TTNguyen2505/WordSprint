#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantMap>

/**
 * @brief BridgeController
 *
 * QObject-derived controller exposed to the React frontend through Qt WebChannel.
 * Implements the communication contract specified in Chapter 2.4 and Chapter 2.5:
 *  - startStage: queries stage configuration and word items, returning JSON array to React.
 *  - completeStage: saves record, updates user progress, unlocks next stage, returning summary JSON.
 *  - processAnswer: evaluates submitted puzzle answer and returns score/timer updates.
 *  - processMessage: universal dispatcher for JSON actions from the web frontend.
 */
class BridgeController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)

public:
    explicit BridgeController(QObject *parent = nullptr);
    ~BridgeController() override = default;

    bool isReady() const { return m_isReady; }

public slots:
    /**
     * @brief Start a game stage.
     * @param requestJson JSON string, e.g. { "stageId": 1 }
     * @return JSON string response containing stage parameters and word items list.
     */
    Q_INVOKABLE QString startStage(const QString &requestJson);

    /**
     * @brief Complete a stage and persist progress.
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
     * @brief Persist user progress state.
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

    // Safe JSON conversion helpers
    QJsonObject parseJson(const QString &jsonStr) const;
    QString buildResponse(const QJsonObject &obj) const;
    QString buildErrorResponse(const QString &errorMessage) const;

    // Fallback word dataset matching Chapter 2.5 schema when DB table is not yet seeded
    QJsonArray generateFallbackWords(int stageId, int count) const;
};
