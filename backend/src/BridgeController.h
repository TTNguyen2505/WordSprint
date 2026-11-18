#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>

// Repository interfaces already provided in the backend
#include "DatabaseManager.h"
#include "IVocabularyRepository.h"
#include "VocabularyRepository.h"

/**
 * @brief BridgeController
 *
 * QObject‑derived class that is exposed to the frontend through Qt WebChannel.
 * It provides Q_INVOKABLE methods which the React UI can call via the
 * JavaScript `bridge` object (registered as "bridge" in QWebChannel).
 *
 * The methods accept JSON strings (encoded by the frontend) and return a JSON
 * string as response, matching the contract described in Chapter 2 of the
 * project report (system design).
 */
class BridgeController : public QObject {
    Q_OBJECT
public:
    explicit BridgeController(QObject *parent = nullptr);

    /**
     * @brief Initialise a game stage.
     *
     * Expected input JSON: { "stageId": <int> }
     * Returns JSON containing stage configuration and the list of words to be
     * used for the puzzles.
     */
    Q_INVOKABLE QString startStage(const QString &requestJson);

    /**
     * @brief Mark a stage as finished.
     *
     * Expected input JSON: { "stageId": <int>, "score": <int>, "time": <int> }
     * Returns JSON with the newly created record identifier and updated high‑score.
     */
    Q_INVOKABLE QString completeStage(const QString &requestJson);

    /**
     * Placeholder for future extensions – e.g. persisting user progress.
     */
    Q_INVOKABLE QString saveProgress(const QString &requestJson);
    Q_INVOKABLE QString loadProgress(const QString &requestJson);

signals:
    // Emitted after a stage has been successfully started
    void stageStarted(int stageId);
    // Emitted after a stage has been completed with the calculated score
    void stageCompleted(int stageId, int score);
    // General purpose signal for sending arbitrary JSON back to the UI
    void jsonMessage(const QString &payload);

private:
    std::shared_ptr<DatabaseManager> m_dbMgr;
    std::unique_ptr<IVocabularyRepository> m_vocabRepo;
    // Helper to parse incoming JSON safely
    QJsonObject parseJson(const QString &jsonStr) const;
    // Helper to build a JSON reply string
    QString buildResponse(const QJsonObject &obj) const;
};
