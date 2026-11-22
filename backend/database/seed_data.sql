PRAGMA foreign_keys = ON;

INSERT OR IGNORE INTO users (user_id, username) VALUES
    (1, 'Sample Player');

INSERT OR IGNORE INTO user_settings (user_id) VALUES
    (1);

INSERT OR IGNORE INTO topics (topic_id, topic_name, description) VALUES
    (1, 'Daily Communication', 'Everyday greetings, messages, and conversations.'),
    (2, 'Travel & Culture', 'Travel planning, transport, and cultural places.'),
    (3, 'Technology & IT', 'Digital devices, software, and information technology.'),
    (4, 'Business & Career', 'Workplace communication, meetings, and professional growth.'),
    (5, 'Academic & Science', 'Scientific concepts, research, and learning.');

INSERT OR IGNORE INTO stages (
    stage_id, topic_id, cefr_level, unlock_order, time_limit, question_count
) VALUES
    (1, 1, 'A1', 1, 60, 1),
    (2, 2, 'A1', 2, 60, 1),
    (3, 3, 'A1', 3, 60, 1),
    (4, 4, 'A1', 4, 60, 1),
    (5, 5, 'A1', 5, 60, 1),
    (6, 1, 'A2', 6, 60, 1),
    (7, 2, 'A2', 7, 60, 1),
    (8, 3, 'A2', 8, 60, 1),
    (9, 4, 'A2', 9, 60, 1),
    (10, 5, 'A2', 10, 60, 1),
    (11, 1, 'B1', 11, 60, 1),
    (12, 2, 'B1', 12, 60, 1),
    (13, 3, 'B1', 13, 60, 1),
    (14, 4, 'B1', 14, 60, 1),
    (15, 5, 'B1', 15, 60, 1),
    (16, 1, 'B2', 16, 60, 1),
    (17, 2, 'B2', 17, 60, 1),
    (18, 3, 'B2', 18, 60, 1),
    (19, 4, 'B2', 19, 60, 1),
    (20, 5, 'B2', 20, 60, 1),
    (21, 1, 'C1', 21, 60, 1),
    (22, 2, 'C1', 22, 60, 1),
    (23, 3, 'C1', 23, 60, 1),
    (24, 4, 'C1', 24, 60, 1),
    (25, 5, 'C1', 25, 60, 1),
    (26, 1, 'C2', 26, 60, 1),
    (27, 2, 'C2', 27, 60, 1),
    (28, 3, 'C2', 28, 60, 1),
    (29, 4, 'C2', 29, 60, 1),
    (30, 5, 'C2', 30, 60, 1);

INSERT OR IGNORE INTO word_pool (
    word_id, topic_id, cefr_level, target_word, hint_text, image_path, audio_path
) VALUES
    (1, 1, 'A1', 'hello', 'xin chào', NULL, 'assets/audio/hello.mp3'),
    (2, 2, 'A1', 'ticket', 'vé', NULL, 'assets/audio/ticket.mp3'),
    (3, 3, 'A1', 'screen', 'màn hình', NULL, 'assets/audio/screen.mp3'),
    (4, 4, 'A1', 'salary', 'tiền lương', NULL, 'assets/audio/salary.mp3'),
    (5, 5, 'A1', 'plant', 'thực vật; cây', NULL, 'assets/audio/plant.mp3'),
    (6, 1, 'A2', 'message', 'tin nhắn; lời nhắn', NULL, 'assets/audio/message.mp3'),
    (7, 2, 'A2', 'journey', 'cuộc hành trình', NULL, 'assets/audio/journey.mp3'),
    (8, 3, 'A2', 'download', 'tải xuống', NULL, 'assets/audio/download.mp3'),
    (9, 4, 'A2', 'meeting', 'cuộc họp', NULL, 'assets/audio/meeting.mp3'),
    (10, 5, 'A2', 'energy', 'năng lượng', NULL, 'assets/audio/energy.mp3'),
    (11, 1, 'B1', 'explain', 'giải thích', NULL, 'assets/audio/explain.mp3'),
    (12, 2, 'B1', 'luggage', 'hành lý', NULL, 'assets/audio/luggage.mp3'),
    (13, 3, 'B1', 'update', 'bản cập nhật', NULL, 'assets/audio/update.mp3'),
    (14, 4, 'B1', 'colleague', 'đồng nghiệp', NULL, 'assets/audio/colleague.mp3'),
    (15, 5, 'B1', 'evidence', 'bằng chứng', NULL, 'assets/audio/evidence.mp3'),
    (16, 1, 'B2', 'negotiate', 'đàm phán; thương lượng', NULL, 'assets/audio/negotiate.mp3'),
    (17, 2, 'B2', 'itinerary', 'lịch trình chuyến đi', NULL, 'assets/audio/itinerary.mp3'),
    (18, 3, 'B2', 'encrypt', 'mã hóa', NULL, 'assets/audio/encrypt.mp3'),
    (19, 4, 'B2', 'revenue', 'doanh thu', NULL, 'assets/audio/revenue.mp3'),
    (20, 5, 'B2', 'hypothesis', 'giả thuyết', NULL, 'assets/audio/hypothesis.mp3'),
    (21, 1, 'C1', 'articulate', 'diễn đạt rõ ràng', NULL, 'assets/audio/articulate.mp3'),
    (22, 2, 'C1', 'sustainable', 'bền vững', NULL, 'assets/audio/sustainable.mp3'),
    (23, 3, 'C1', 'vulnerability', 'lỗ hổng bảo mật; điểm yếu', NULL, 'assets/audio/vulnerability.mp3'),
    (24, 4, 'C1', 'appraisal', 'sự đánh giá; buổi đánh giá', NULL, 'assets/audio/appraisal.mp3'),
    (25, 5, 'C1', 'empirical', 'dựa trên thực nghiệm', NULL, 'assets/audio/empirical.mp3'),
    (26, 1, 'C2', 'conciliatory', 'có tính hòa giải; xoa dịu', NULL, 'assets/audio/conciliatory.mp3'),
    (27, 2, 'C2', 'confluence', 'nơi hợp lưu; sự hội tụ', NULL, 'assets/audio/confluence.mp3'),
    (28, 3, 'C2', 'obfuscate', 'làm cho khó hiểu; che giấu', NULL, 'assets/audio/obfuscate.mp3'),
    (29, 4, 'C2', 'fiduciary', 'người có trách nhiệm ủy thác', NULL, 'assets/audio/fiduciary.mp3'),
    (30, 5, 'C2', 'corroborate', 'xác nhận bằng chứng bổ trợ', NULL, 'assets/audio/corroborate.mp3');

INSERT OR IGNORE INTO user_progress (
    user_id, stage_id, is_unlocked, is_completed, high_score, best_time
) VALUES
    (1, 1, 1, 0, 0, NULL),
    (1, 2, 0, 0, 0, NULL),
    (1, 3, 0, 0, 0, NULL),
    (1, 4, 0, 0, 0, NULL),
    (1, 5, 0, 0, 0, NULL),
    (1, 6, 0, 0, 0, NULL),
    (1, 7, 0, 0, 0, NULL),
    (1, 8, 0, 0, 0, NULL),
    (1, 9, 0, 0, 0, NULL),
    (1, 10, 0, 0, 0, NULL),
    (1, 11, 0, 0, 0, NULL),
    (1, 12, 0, 0, 0, NULL),
    (1, 13, 0, 0, 0, NULL),
    (1, 14, 0, 0, 0, NULL),
    (1, 15, 0, 0, 0, NULL),
    (1, 16, 0, 0, 0, NULL),
    (1, 17, 0, 0, 0, NULL),
    (1, 18, 0, 0, 0, NULL),
    (1, 19, 0, 0, 0, NULL),
    (1, 20, 0, 0, 0, NULL),
    (1, 21, 0, 0, 0, NULL),
    (1, 22, 0, 0, 0, NULL),
    (1, 23, 0, 0, 0, NULL),
    (1, 24, 0, 0, 0, NULL),
    (1, 25, 0, 0, 0, NULL),
    (1, 26, 0, 0, 0, NULL),
    (1, 27, 0, 0, 0, NULL),
    (1, 28, 0, 0, 0, NULL),
    (1, 29, 0, 0, 0, NULL),
    (1, 30, 0, 0, 0, NULL);
