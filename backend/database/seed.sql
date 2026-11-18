-- seed.sql for English Vocabulary system
PRAGMA foreign_keys = ON;

-- Insert levels
INSERT OR IGNORE INTO levels (code) VALUES ('A1'),('A2'),('B1'),('B2'),('C1'),('C2');

-- Insert topics
INSERT OR IGNORE INTO topics (name) VALUES
    ('Daily Life'),
    ('Travel & Transport'),
    ('Education & Work'),
    ('Technology'),
    ('Environment & Science'),
    ('Health & Lifestyle'),
    ('Business'),
    ('Society & Culture');

-- Helper: get level and topic ids (subqueries used in each insert)
INSERT OR IGNORE INTO vocabularies (word, phonetic, part_of_speech, meaning_vi, definition_en, example_sentence, example_translation, level_id, topic_id)
VALUES
    ('cat','/kæt/','noun','con mèo','A small domesticated carnivorous mammal.','The cat sat on the mat.','Con mèo ngồi trên thảm.', (SELECT id FROM levels WHERE code='A1'), (SELECT id FROM topics WHERE name='Daily Life')),
    ('run','/rʌn/','verb','chạy','To move at a speed faster than a walk.','She runs every morning.','Cô ấy chạy mỗi sáng.', (SELECT id FROM levels WHERE code='A1'), (SELECT id FROM topics WHERE name='Health & Lifestyle')),
    ('book','/bʊk/','noun','sách','A written or printed work.','I bought a new book.','Tôi mua một cuốn sách mới.', (SELECT id FROM levels WHERE code='A1'), (SELECT id FROM topics WHERE name='Education & Work')),
    ('airport','/ˈɛərpɔrt/','noun','sân bay','A complex of runways and buildings for aircraft.','We arrived at the airport early.','Chúng tôi đến sân bay sớm.', (SELECT id FROM levels WHERE code='A2'), (SELECT id FROM topics WHERE name='Travel & Transport')),
    ('computer','/kəmˈpjuːtər/','noun','máy tính','An electronic device for storing and processing data.','My computer crashes often.','Máy tính của tôi thường bị treo.', (SELECT id FROM levels WHERE code='A2'), (SELECT id FROM topics WHERE name='Technology')),
    ('environment','/ɪnˈvaɪrənmənt/','noun','môi trường','The surroundings or conditions in which a person lives.','We must protect the environment.','Chúng ta phải bảo vệ môi trường.', (SELECT id FROM levels WHERE code='B1'), (SELECT id FROM topics WHERE name='Environment & Science')),
    ('entrepreneur','/ˌɑːntrəprəˈnɜːr/','noun','doanh nhân','A person who starts and runs a business.','She is a successful entrepreneur.','Cô ấy là một doanh nhân thành công.', (SELECT id FROM levels WHERE code='B2'), (SELECT id FROM topics WHERE name='Business')),
    ('philosophy','/fɪˈlɒsəfi/','noun','triết học','The study of fundamental nature of knowledge.','He studies philosophy at university.','Anh ấy học triết học tại trường đại học.', (SELECT id FROM levels WHERE code='C1'), (SELECT id FROM topics WHERE name='Society & Culture')),
    ('quantum','/ˈkwɒntəm/','noun','định lượng','The smallest amount of a physical quantity.','Quantum mechanics is complex.','Cơ học lượng tử rất phức tạp.', (SELECT id FROM levels WHERE code='C2'), (SELECT id FROM topics WHERE name='Technology')),
    ('sustainability','/səˌsteɪnəˈbɪlɪti/','noun','tính bền vững','Avoidance of the depletion of natural resources.','Sustainability is key for future generations.','Tính bền vững là chìa khóa cho các thế hệ tương lai.', (SELECT id FROM levels WHERE code='C2'), (SELECT id FROM topics WHERE name='Environment & Science'));
