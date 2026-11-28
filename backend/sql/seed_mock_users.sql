PRAGMA foreign_keys = ON;

INSERT OR IGNORE INTO users (user_id, username) VALUES
    (1, 'Sample Player'),
    (2, 'Alex');

INSERT OR IGNORE INTO stage_records (record_id, user_id, stage_id, score, completion_time) VALUES
    (1, 1, 1, 62, 54),
    (2, 1, 1, 86, 38),
    (3, 1, 2, 111, 42),
    (4, 1, 3, 145, 57),
    (5, 1, 1, 86, 45);
