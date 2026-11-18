-- Enable foreign key support
PRAGMA foreign_keys = ON;

-- Topics table stores vocabulary topic categories
CREATE TABLE IF NOT EXISTS topics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- Words table stores each vocabulary entry
CREATE TABLE IF NOT EXISTS words (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    topic_id INTEGER NOT NULL,
    word TEXT NOT NULL,
    phonetic TEXT,
    pos TEXT, -- part of speech
    meaning_en TEXT,
    meaning_vi TEXT,
    example TEXT,
    cefr_level TEXT NOT NULL,
    CONSTRAINT fk_topic FOREIGN KEY (topic_id) REFERENCES topics(id) ON DELETE CASCADE,
    CONSTRAINT chk_cefr CHECK (cefr_level IN ('A1','A2','B1','B2','C1','C2'))
);

-- Indexes for fast lookup by level and topic
CREATE INDEX IF NOT EXISTS idx_words_cefr ON words(cefr_level);
CREATE INDEX IF NOT EXISTS idx_words_topic ON words(topic_id);
