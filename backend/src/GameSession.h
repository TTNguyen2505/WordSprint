#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <vector>

/**
 * @brief GameSession
 *
 * Represents the runtime state of a single game play for a user.
 * It is not persisted – persistence is handled by the repository layer
 * (user_progress / stage_records tables). The session lives only while the
 * user is interacting with a particular stage.
 *
 * Fields are aligned with the database schema described in Chapter 2:
 *   - stageId            – identifier of the current stage (stages.stage_id)
 *   - wordIds            – list of word_pool.word_id that belong to the stage
 *   - words              – the actual word objects (target_word, hint_text,…)
 *   - timeLimit          – stages.time_limit (seconds)
 *   - questionCount      – stages.question_count
 *   - currentScore       – accumulated score for the session
 *   - elapsedTime        – seconds elapsed since startStage was called
 */
class GameSession : public QObject
{
    Q_OBJECT
public:
    explicit GameSession(QObject *parent = nullptr)
        : QObject(parent)
        , m_stageId(-1)
        , m_timeLimit(60)
        , m_questionCount(10)
        , m_currentScore(0)
        , m_elapsedTime(0)
    {}

    // Setters & getters -----------------------------------------------------
    void setStageId(int id) { m_stageId = id; }
    int stageId() const { return m_stageId; }

    void setWordIds(const std::vector<int> &ids) { m_wordIds = ids; }
    const std::vector<int>& wordIds() const { return m_wordIds; }

    void setWords(const QJsonArray &arr) { m_words = arr; }
    const QJsonArray& words() const { return m_words; }

    void setTimeLimit(int seconds) { m_timeLimit = seconds; }
    int timeLimit() const { return m_timeLimit; }

    void setQuestionCount(int count) { m_questionCount = count; }
    int questionCount() const { return m_questionCount; }

    void addScore(int delta) { m_currentScore += delta; }
    int currentScore() const { return m_currentScore; }

    void setElapsedTime(int secs) { m_elapsedTime = secs; }
    int elapsedTime() const { return m_elapsedTime; }

    /**
     * @brief Convert the session state to a JSON object that matches the
     *        frontend expectations (see Chapter 2, section "JSON response").
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["stageId"] = m_stageId;
        obj["timeLimit"] = m_timeLimit;
        obj["questionCount"] = m_questionCount;
        obj["score"] = m_currentScore;
        obj["elapsedTime"] = m_elapsedTime;
        obj["words"] = m_words; // already a JSON array of word objects
        return obj;
    }

private:
    int m_stageId;
    std::vector<int> m_wordIds; // internal ids for lookup, not sent to UI
    QJsonArray m_words;         // full word objects (target_word, hint, etc.)
    int m_timeLimit;
    int m_questionCount;
    int m_currentScore;
    int m_elapsedTime;
};
