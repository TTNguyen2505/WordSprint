-- Insert topics
INSERT INTO topics (name) VALUES ('Daily Life');
INSERT INTO topics (name) VALUES ('Travel & Culture');
INSERT INTO topics (name) VALUES ('Technology');
INSERT INTO topics (name) VALUES ('Business & Work');
INSERT INTO topics (name) VALUES ('Science & Environment');
INSERT INTO topics (name) VALUES ('Academic & Literature');

-- Insert words (10 per CEFR level, spread across topics)
-- Helper to fetch topic id by name
-- A1 level words
INSERT INTO words (topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level) VALUES
((SELECT id FROM topics WHERE name='Daily Life'), 'hello', 'həˈloʊ', 'interjection', 'greeting', 'chào', 'Hello, how are you?', 'A1'),
((SELECT id FROM topics WHERE name='Daily Life'), 'home', 'hoʊm', 'noun', 'place where one lives', 'nhà', 'I am going home.', 'A1'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'airport', 'ˈɛərpɔrt', 'noun', 'place where planes land', 'sân bay', 'We arrived at the airport.', 'A1'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'passport', 'ˈpæspɔrt', 'noun', 'official travel document', 'hộ chiếu', 'Don''t forget your passport.', 'A1'),
((SELECT id FROM topics WHERE name='Technology'), 'computer', 'kəmˈpjuːtər', 'noun', 'electronic device', 'máy tính', 'My computer is fast.', 'A1'),
((SELECT id FROM topics WHERE name='Technology'), 'software', 'ˈsɒftwɛər', 'noun', 'programs', 'phần mềm', 'Install the software.', 'A1'),
((SELECT id FROM topics WHERE name='Business & Work'), 'meeting', 'ˈmiːtɪŋ', 'noun', 'gathering for discussion', 'cuộc họp', 'We have a meeting.', 'A1'),
((SELECT id FROM topics WHERE name='Business & Work'), 'contract', 'ˈkɒntrækt', 'noun', 'legal agreement', 'hợp đồng', 'Sign the contract.', 'A1'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'planet', 'ˈplænɪt', 'noun', 'celestial body', 'hành tinh', 'Earth is a planet.', 'A1'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'climate', 'ˈklaɪmət', 'noun', 'weather patterns', 'khí hậu', 'Climate change.', 'A1');

-- A2 level words
INSERT INTO words (topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level) VALUES
((SELECT id FROM topics WHERE name='Daily Life'), 'breakfast', 'ˈbrɛkfəst', 'noun', 'first meal of the day', 'bữa sáng', 'I ate breakfast.', 'A2'),
((SELECT id FROM topics WHERE name='Daily Life'), 'schedule', 'ˈskɛdʒuːl', 'noun', 'plan of activities', 'lịch trình', 'Check the schedule.', 'A2'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'tourist', 'ˈtʊrɪst', 'noun', 'traveler for pleasure', 'du khách', 'The city attracts many tourists.', 'A2'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'customs', 'ˈkʌstəmz', 'noun', 'border control', 'hải quan', 'Pass through customs.', 'A2'),
((SELECT id FROM topics WHERE name='Technology'), 'algorithm', 'ˈælɡərɪðəm', 'noun', 'step‑by‑step procedure', 'thuật toán', 'Sorting algorithm.', 'A2'),
((SELECT id FROM topics WHERE name='Technology'), 'network', 'ˈnɛtwɜːk', 'noun', 'system of interconnected computers', 'mạng', 'Check the network.', 'A2'),
((SELECT id FROM topics WHERE name='Business & Work'), 'agenda', 'əˈdʒɛndə', 'noun', 'list of items to discuss', 'lịch trình họp', 'The agenda is clear.', 'A2'),
((SELECT id FROM topics WHERE name='Business & Work'), 'invoice', 'ˈɪnvɔɪs', 'noun', 'bill for goods/services', 'hóa đơn', 'Send the invoice.', 'A2'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'biodiversity', 'ˌbaɪoʊdaɪˈvɜːrsɪti', 'noun', 'variety of life', 'đa dạng sinh học', 'Protect biodiversity.', 'A2'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'recycle', 'riˈsaɪkəl', 'verb', 'process used materials again', 'tái chế', 'We recycle waste.', 'A2');

-- B1 level words
INSERT INTO words (topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level) VALUES
((SELECT id FROM topics WHERE name='Daily Life'), 'relationship', 'rɪˈleɪʃənʃɪp', 'noun', 'connection between people', 'mối quan hệ', 'Their relationship is strong.', 'B1'),
((SELECT id FROM topics WHERE name='Daily Life'), 'responsibility', 'rɪˌspɒnsɪˈbɪlɪti', 'noun', 'duty', 'trách nhiệm', 'Take responsibility.', 'B1'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'itinerary', 'aɪˈtɪnərəri', 'noun', 'planned route', 'lịch trình', 'Check the itinerary.', 'B1'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'souvenir', 'ˌsuːvəˈnɪər', 'noun', 'keepsake from a trip', 'quà lưu niệm', 'Buy a souvenir.', 'B1'),
((SELECT id FROM topics WHERE name='Technology'), 'virtualization', 'ˌvɜːrtjʊəlɪˈzeɪʃən', 'noun', 'creating virtual versions', 'ảo hoá', 'Server virtualization.', 'B1'),
((SELECT id FROM topics WHERE name='Technology'), 'encryption', 'ɪnˈkrɪpʃən', 'noun', 'process of encoding data', 'mã hoá', 'Data encryption is essential.', 'B1'),
((SELECT id FROM topics WHERE name='Business & Work'), 'logistics', 'ləˈdʒɪstɪks', 'noun', 'coordination of operations', 'logistics', 'Logistics management.', 'B1'),
((SELECT id FROM topics WHERE name='Business & Work'), 'benchmark', 'ˈbɛntʃmɑːrk', 'noun', 'standard for comparison', 'điểm chuẩn', 'Set a benchmark.', 'B1'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'photosynthesis', 'ˌfoʊtəʊˈsɪnθəsɪs', 'noun', 'process plants use to make food', 'quang hợp', 'Photosynthesis converts light.', 'B1'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'ecosystem', 'ˈiːkəʊˌsɪstəm', 'noun', 'community of organisms', 'hệ sinh thái', 'A healthy ecosystem.', 'B1');

-- B2 level words
INSERT INTO words (topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level) VALUES
((SELECT id FROM topics WHERE name='Daily Life'), 'consciousness', 'ˈkɒnʃəsnəs', 'noun', 'state of being aware', 'ý thức', 'He regained consciousness.', 'B2'),
((SELECT id FROM topics WHERE name='Daily Life'), 'metamorphosis', 'ˌmɛtəˈmɔːfəsɪs', 'noun', 'transformation', 'sự biến đổi', 'Butterflies undergo metamorphosis.', 'B2'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'heritage', 'ˈhɛrɪtɪdʒ', 'noun', 'cultural inheritance', 'di sản', 'World heritage sites.', 'B2'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'itinerary', 'aɪˈtɪnərəri', 'noun', 'planned route', 'lịch trình', 'Check the itinerary.', 'B2'),
((SELECT id FROM topics WHERE name='Technology'), 'cryptocurrency', 'krɪptəʊˈkʌrənsi', 'noun', 'digital currency', 'tiền cryptocurrency', 'Bitcoin is a cryptocurrency.', 'B2'),
((SELECT id FROM topics WHERE name='Technology'), 'quantumcomputing', 'ˈkwɒntəm kəmˈpjuːtɪŋ', 'noun', 'computing using quantum bits', 'tính toán lượng tử', 'Quantum computing advances.', 'B2'),
((SELECT id FROM topics WHERE name='Business & Work'), 'entrepreneurship', 'ˌɒntrəprəˈnɜːrʃɪp', 'noun', 'starting businesses', 'khởi nghiệp', 'Entrepreneurship is risky.', 'B2'),
((SELECT id FROM topics WHERE name='Business & Work'), 'jurisprudence', 'ˌdʒʊərɪsˈpruːdəns', 'noun', 'theory of law', 'luật học', 'Study jurisprudence.', 'B2'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'nanotechnology', 'ˌnaɪnəʊtɛkˈnɒlədʒi', 'noun', 'manipulation at nanoscale', 'công nghệ nano', 'Nanotechnology advances.', 'B2'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'anthropogenic', 'ˌænθrəpəˈdʒɛnɪk', 'adj', 'caused by humans', 'do do người', 'Anthropogenic emissions.', 'B2');

-- C1 level words
INSERT INTO words (topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level) VALUES
((SELECT id FROM topics WHERE name='Daily Life'), 'procrastination', 'proʊˌkræstɪˈneɪʃən', 'noun', 'delaying action', 'sự trì hoãn', 'Procrastination hurts productivity.', 'C1'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'cultural', 'ˈkʌltʃərəl', 'adj', 'relating to culture', 'văn hoá', 'Cultural exchange programs.', 'C1'),
((SELECT id FROM topics WHERE name='Technology'), 'blockchain', 'ˈblɒkˌtʃeɪn', 'noun', 'distributed ledger technology', 'chuỗi khối', 'Blockchain improves transparency.', 'C1'),
((SELECT id FROM topics WHERE name='Business & Work'), 'strategic', 'strəˈtiːdʒɪk', 'adj', 'relating to a plan', 'chiến lược', 'Strategic planning is essential.', 'C1'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'climatechange', 'ˈklaɪmət tʃeɪndʒ', 'noun', 'global warming phenomenon', 'biến đổi khí hậu', 'Climate change impacts us all.', 'C1'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'sustainability', 'səˌsteɪnəˈbɪlɪti', 'noun', 'maintaining resources', 'bảo tồn', 'Focus on sustainability.', 'C1'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'analysis', 'əˈnæləsɪs', 'noun', 'detailed examination', 'phân tích', 'Critical analysis.', 'C1'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'hypothesis', 'haɪˈpɒθəsɪs', 'noun', 'proposed explanation', 'giả thuyết', 'Test the hypothesis.', 'C1'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'methodology', 'ˌmɛθəˈdɒlədʒi', 'noun', 'system of methods', 'phương pháp luận', 'Research methodology.', 'C1'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'citation', 'saɪˈteɪʃən', 'noun', 'reference to a source', 'trích dẫn', 'Add a citation.', 'C1');

-- C2 level words
INSERT INTO words (topic_id, word, phonetic, pos, meaning_en, meaning_vi, example, cefr_level) VALUES
((SELECT id FROM topics WHERE name='Daily Life'), 'epistemology', 'ɪˌpɪstəˈmɒlədʒi', 'noun', 'study of knowledge', 'luận lý học', 'Epistemology explores knowledge.', 'C2'),
((SELECT id FROM topics WHERE name='Travel & Culture'), 'anthropology', 'ˌænθrəˈpɒlədʒi', 'noun', 'study of humans', 'nhân chủng học', 'She studies anthropology.', 'C2'),
((SELECT id FROM topics WHERE name='Technology'), 'hyperconvergence', 'ˌhaɪpərkənˈvɜːrdʒəns', 'noun', 'integrated IT infrastructure', 'công nghệ siêu hội tụ', 'Hyperconvergence simplifies data centers.', 'C2'),
((SELECT id FROM topics WHERE name='Business & Work'), 'jurisprudence', 'ˌdʒʊərɪsˈpruːdəns', 'noun', 'theory of law', 'luật học', 'Study jurisprudence.', 'C2'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'quantummechanics', 'ˈkwɒntəm məˈkænɪks', 'noun', 'physics of subatomic particles', 'cơ học lượng tử', 'Quantum mechanics is complex.', 'C2'),
((SELECT id FROM topics WHERE name='Science & Environment'), 'biogeochemistry', 'ˌbaɪoʊdʒiˈkɛmɪstri', 'noun', 'study of chemical cycles', 'địa hoá sinh học', 'Biogeochemistry examines cycles.', 'C2'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'hermeneutics', 'ˌhɜːrməˈnjuːtɪks', 'noun', 'interpretation theory', 'giải thích', 'Study hermeneutics.', 'C2'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'postmodernism', 'ˈpoʊstˈmɒdərnɪzəm', 'noun', 'cultural movement', 'hậu hiện đại', 'Postmodernism in literature.', 'C2'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'metacognition', 'ˌmɛtəˌkɒɡˈnɪʃən', 'noun', 'thinking about thinking', 'siêu nhận thức', 'Metacognition improves learning.', 'C2'),
((SELECT id FROM topics WHERE name='Academic & Literature'), 'dialectics', 'daɪˈælɛktɪks', 'noun', 'dialogue of opposites', 'phép biện chứng', 'Dialectics drives philosophy.', 'C2');
