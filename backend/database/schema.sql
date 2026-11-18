-- schema.sql for English Vocabulary system
PRAGMA foreign_keys = ON;

-- Levels table (CEFR)
CREATE TABLE IF NOT EXISTS levels (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    code TEXT NOT NULL UNIQUE CHECK (code IN ('A1','A2','B1','B2','C1','C2'))
);

-- Topics table
CREATE TABLE IF NOT EXISTS topics (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- Vocabularies table
CREATE TABLE IF NOT EXISTS vocabularies (
    id                 INTEGER PRIMARY KEY AUTOINCREMENT,
    word               TEXT NOT NULL,
    phonetic           TEXT,
    part_of_speech    TEXT,
    meaning_vi         TEXT,
    definition_en      TEXT,
    example_sentence   TEXT,
    example_translation TEXT,
    level_id           INTEGER NOT NULL,
    topic_id           INTEGER NOT NULL,
    created_at         TEXT DEFAULT (datetime('now')),
    FOREIGN KEY (level_id) REFERENCES levels(id) ON DELETE CASCADE,
    FOREIGN KEY (topic_id) REFERENCES topics(id) ON DELETE CASCADE,
    UNIQUE(word, level_id, topic_id)
);

-- Indexes for fast lookup
CREATE INDEX IF NOT EXISTS idx_vocab_level ON vocabularies(level_id);
CREATE INDEX IF NOT EXISTS idx_vocab_topic ON vocabularies(topic_id);
