PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS topics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT
);

CREATE TABLE IF NOT EXISTS vocabularies (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_id INTEGER NOT NULL,
    word TEXT NOT NULL,
    phonetic TEXT,
    part_of_speech TEXT,
    cefr_level TEXT NOT NULL CHECK (cefr_level IN ('A1', 'A2', 'B1', 'B2', 'C1', 'C2')),
    meaning TEXT NOT NULL,
    example_sentence TEXT,
    FOREIGN KEY (topic_id) REFERENCES topics(id)
);

CREATE TABLE IF NOT EXISTS learning_progress (
    vocab_id INTEGER PRIMARY KEY,
    correct_count INTEGER NOT NULL DEFAULT 0,
    wrong_count INTEGER NOT NULL DEFAULT 0,
    mastery_level INTEGER NOT NULL DEFAULT 0,
    last_reviewed TEXT,
    FOREIGN KEY (vocab_id) REFERENCES vocabularies(id)
);
