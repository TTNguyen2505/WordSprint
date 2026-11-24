#pragma once

#include "../models/Stage.h"
#include "../models/StageRecord.h"
#include "../models/User.h"
#include "../models/WordItem.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <optional>
#include <string>

namespace SqliteRepositoryUtils {
    inline bool databaseAvailable(const QSqlDatabase &database, const char *repository, const char *operation) {
        if (database.isOpen()) {
            return true;
        }
        qWarning() << repository << "::" << operation << "could not run because the database connection is not open.";
        return false;
    }

    inline void warnQueryFailure(const char *repository, const char *operation, const QSqlQuery &query) {
        qWarning().noquote() << repository << "::" << operation << "failed:" << query.lastError().text();
    }

    inline std::string stringColumn(const QSqlQuery &query, int column) {
        const QVariant value = query.value(column);
        return value.isNull() ? std::string{} : value.toString().toStdString();
    }

    inline Stage mapStage(const QSqlQuery &query) {
        Stage stage;
        stage.stage_id = query.value(0).toInt();
        stage.topic_id = query.value(1).toInt();
        stage.cefr_level = stringColumn(query, 2);
        stage.time_limit = query.value(3).toInt();
        stage.question_count = query.value(4).toInt();
        stage.pass_score = query.value(5).toInt();
        return stage;
    }

    inline WordItem mapWordItem(const QSqlQuery &query) {
        WordItem word;
        word.word_id = query.value(0).toInt();
        word.topic_id = query.value(1).toInt();
        word.cefr_level = stringColumn(query, 2);
        word.target_word = stringColumn(query, 3);
        word.hint_image_path = stringColumn(query, 4);
        const QVariant audioPath = query.value(5);
        if (!audioPath.isNull()) {
            word.audio_path = audioPath.toString().toStdString();
        }
        return word;
    }

    inline StageRecord mapStageRecord(const QSqlQuery &query) {
        StageRecord record;
        record.record_id = query.value(0).toInt();
        record.user_id = query.value(1).toInt();
        record.stage_id = query.value(2).toInt();
        record.score = query.value(3).toInt();
        record.completion_time = query.value(4).toInt();
        record.played_at = query.value(5).toLongLong();
        return record;
    }

    inline User mapUser(const QSqlQuery &query) {
        User user;
        user.user_id = query.value(0).toInt();
        user.username = stringColumn(query, 1);
        user.created_at = query.value(2).toLongLong();
        user.bgm_enabled = query.value(3).toBool();
        user.sfx_enabled = query.value(4).toBool();
        user.bgm_volume = query.value(5).toInt();
        user.sfx_volume = query.value(6).toInt();
        return user;
    }

} // namespace SqliteRepositoryUtils
