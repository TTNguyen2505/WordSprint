PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    created_at INTEGER NOT NULL DEFAULT (CAST(strftime('%s', 'now') AS INTEGER)),
    bgm_enabled INTEGER NOT NULL DEFAULT 1 CHECK (bgm_enabled IN (0, 1)),
    sfx_enabled INTEGER NOT NULL DEFAULT 1 CHECK (sfx_enabled IN (0, 1)),
    bgm_volume INTEGER NOT NULL DEFAULT 100 CHECK (bgm_volume BETWEEN 0 AND 100),
    sfx_volume INTEGER NOT NULL DEFAULT 100 CHECK (sfx_volume BETWEEN 0 AND 100)
);

CREATE TABLE IF NOT EXISTS topics (
    topic_id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS stages (
    stage_id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_id INTEGER NOT NULL,
    cefr_level TEXT NOT NULL CHECK (cefr_level IN ('A1', 'A2', 'B1', 'B2', 'C1', 'C2')),
    time_limit INTEGER NOT NULL CHECK (time_limit > 0),
    question_count INTEGER NOT NULL CHECK (question_count > 0),
    pass_score INTEGER NOT NULL CHECK (pass_score >= 0),
    FOREIGN KEY (topic_id) REFERENCES topics(topic_id) ON DELETE CASCADE,
    UNIQUE (topic_id, cefr_level)
);

CREATE TABLE IF NOT EXISTS word_pool (
    word_id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_id INTEGER NOT NULL,
    cefr_level TEXT NOT NULL CHECK (cefr_level IN ('A1', 'A2', 'B1', 'B2', 'C1', 'C2')),
    target_word TEXT NOT NULL,
    audio_path TEXT,
    hint_image_path TEXT NOT NULL,
    FOREIGN KEY (topic_id) REFERENCES topics(topic_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS stage_records (
    record_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    stage_id INTEGER NOT NULL,
    score INTEGER NOT NULL,
    completion_time INTEGER NOT NULL,
    played_at INTEGER NOT NULL DEFAULT (CAST(strftime('%s', 'now') AS INTEGER)),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (stage_id) REFERENCES stages(stage_id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_word_pool_topic_cefr
    ON word_pool(topic_id, cefr_level);

CREATE INDEX IF NOT EXISTS idx_stage_records_user_id
    ON stage_records(user_id);

CREATE INDEX IF NOT EXISTS idx_stage_records_leaderboard
    ON stage_records(stage_id, score DESC, completion_time ASC);
