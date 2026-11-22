PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS user_settings (
    user_id INTEGER PRIMARY KEY,
    bgm_enabled INTEGER NOT NULL DEFAULT 1 CHECK (bgm_enabled IN (0, 1)),
    sfx_enabled INTEGER NOT NULL DEFAULT 1 CHECK (sfx_enabled IN (0, 1)),
    bgm_volume INTEGER NOT NULL DEFAULT 80 CHECK (bgm_volume BETWEEN 0 AND 100),
    sfx_volume INTEGER NOT NULL DEFAULT 100 CHECK (sfx_volume BETWEEN 0 AND 100),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS topics (
    topic_id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_name TEXT NOT NULL UNIQUE,
    description TEXT
);

CREATE TABLE IF NOT EXISTS stages (
    stage_id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_id INTEGER NOT NULL,
    cefr_level TEXT NOT NULL CHECK (cefr_level IN ('A1', 'A2', 'B1', 'B2', 'C1', 'C2')),
    unlock_order INTEGER NOT NULL UNIQUE,
    time_limit INTEGER NOT NULL DEFAULT 60,
    question_count INTEGER NOT NULL DEFAULT 10,
    FOREIGN KEY (topic_id) REFERENCES topics(topic_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS word_pool (
    word_id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_id INTEGER NOT NULL,
    cefr_level TEXT NOT NULL CHECK (cefr_level IN ('A1', 'A2', 'B1', 'B2', 'C1', 'C2')),
    target_word TEXT NOT NULL,
    hint_text TEXT,
    image_path TEXT,
    audio_path TEXT NOT NULL,
    FOREIGN KEY (topic_id) REFERENCES topics(topic_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS user_progress (
    user_id INTEGER NOT NULL,
    stage_id INTEGER NOT NULL,
    is_unlocked INTEGER NOT NULL DEFAULT 0 CHECK (is_unlocked IN (0, 1)),
    is_completed INTEGER NOT NULL DEFAULT 0 CHECK (is_completed IN (0, 1)),
    high_score INTEGER NOT NULL DEFAULT 0,
    best_time INTEGER,
    PRIMARY KEY (user_id, stage_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (stage_id) REFERENCES stages(stage_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS stage_records (
    record_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    stage_id INTEGER NOT NULL,
    score INTEGER NOT NULL,
    completion_time INTEGER NOT NULL,
    played_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (stage_id) REFERENCES stages(stage_id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_stages_topic_id
    ON stages(topic_id);

CREATE INDEX IF NOT EXISTS idx_word_pool_topic_cefr
    ON word_pool(topic_id, cefr_level);

CREATE INDEX IF NOT EXISTS idx_user_progress_stage_id
    ON user_progress(stage_id);

CREATE INDEX IF NOT EXISTS idx_stage_records_user_id
    ON stage_records(user_id);

CREATE INDEX IF NOT EXISTS idx_stage_records_leaderboard
    ON stage_records(stage_id, score DESC, completion_time ASC);
