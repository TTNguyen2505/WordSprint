PRAGMA foreign_keys = ON;

INSERT OR IGNORE INTO topics (topic_id, topic_name) VALUES
    (1, 'Daily Communication');

INSERT OR IGNORE INTO stages (
    stage_id, topic_id, cefr_level, time_limit, question_count, pass_score
) VALUES
    (1, 1, 'A1', 90, 10, 70),
    (2, 1, 'A2', 100, 10, 105),
    (3, 1, 'B1', 120, 12, 160),
    (4, 1, 'B2', 135, 12, 225);

INSERT OR IGNORE INTO word_pool (
    word_id, topic_id, cefr_level, target_word, audio_path, hint_image_path
) VALUES
    (1, 1, 'A1', 'hello', 'storage/audio/hello.mp3', 'storage/images/hello.png'),
    (2, 1, 'A2', 'message', 'storage/audio/message.mp3', 'storage/images/message.png'),
    (3, 1, 'B1', 'explain', 'storage/audio/explain.mp3', 'storage/images/explain.png'),
    (4, 1, 'B2', 'negotiate', 'storage/audio/negotiate.mp3', 'storage/images/negotiate.png'),
    (5, 1, 'A1', 'goodbye', 'storage/audio/goodbye.mp3', 'storage/images/goodbye.png'),
    (6, 1, 'A1', 'please', 'storage/audio/please.mp3', 'storage/images/please.png'),
    (7, 1, 'A1', 'thanks', 'storage/audio/thanks.mp3', 'storage/images/thanks.png'),
    (8, 1, 'A1', 'friend', 'storage/audio/friend.mp3', 'storage/images/friend.png'),
    (9, 1, 'A1', 'name', 'storage/audio/name.mp3', 'storage/images/name.png'),
    (10, 1, 'A2', 'invite', 'storage/audio/invite.mp3', 'storage/images/invite.png'),
    (11, 1, 'A2', 'answer', 'storage/audio/answer.mp3', 'storage/images/answer.png'),
    (12, 1, 'A2', 'conversation', 'storage/audio/conversation.mp3', 'storage/images/conversation.png'),
    (13, 1, 'A2', 'suggest', 'storage/audio/suggest.mp3', 'storage/images/suggest.png'),
    (14, 1, 'A2', 'promise', 'storage/audio/promise.mp3', 'storage/images/promise.png'),
    (15, 1, 'B1', 'discuss', 'storage/audio/discuss.mp3', 'storage/images/discuss.png'),
    (16, 1, 'B1', 'mention', 'storage/audio/mention.mp3', 'storage/images/mention.png'),
    (17, 1, 'B1', 'apologize', 'storage/audio/apologize.mp3', 'storage/images/apologize.png'),
    (18, 1, 'B1', 'express', 'storage/audio/express.mp3', 'storage/images/express.png'),
    (19, 1, 'B1', 'opinion', 'storage/audio/opinion.mp3', 'storage/images/opinion.png'),
    (20, 1, 'B2', 'persuade', 'storage/audio/persuade.mp3', 'storage/images/persuade.png'),
    (21, 1, 'B2', 'clarify', 'storage/audio/clarify.mp3', 'storage/images/clarify.png'),
    (22, 1, 'B2', 'emphasize', 'storage/audio/emphasize.mp3', 'storage/images/emphasize.png'),
    (23, 1, 'B2', 'interpret', 'storage/audio/interpret.mp3', 'storage/images/interpret.png'),
    (24, 1, 'B2', 'resolve', 'storage/audio/resolve.mp3', 'storage/images/resolve.png'),
    (25, 1, 'A1', 'smile', 'storage/audio/smile.mp3', 'storage/images/smile.png'),
    (26, 1, 'A1', 'wave', 'storage/audio/wave.mp3', 'storage/images/wave.png'),
    (27, 1, 'A1', 'meet', 'storage/audio/meet.mp3', 'storage/images/meet.png'),
    (28, 1, 'A1', 'talk', 'storage/audio/talk.mp3', 'storage/images/talk.png'),
    (29, 1, 'A2', 'welcome', 'storage/audio/welcome.mp3', 'storage/images/welcome.png'),
    (30, 1, 'A2', 'greet', 'storage/audio/greet.mp3', 'storage/images/greet.png'),
    (31, 1, 'A2', 'listen', 'storage/audio/listen.mp3', 'storage/images/listen.png'),
    (32, 1, 'A2', 'reply', 'storage/audio/reply.mp3', 'storage/images/reply.png'),
    (33, 1, 'B1', 'describe', 'storage/audio/describe.mp3', 'storage/images/describe.png'),
    (34, 1, 'B1', 'request', 'storage/audio/request.mp3', 'storage/images/request.png'),
    (35, 1, 'B1', 'respond', 'storage/audio/respond.mp3', 'storage/images/respond.png'),
    (36, 1, 'B1', 'communicate', 'storage/audio/communicate.mp3', 'storage/images/communicate.png'),
    (37, 1, 'B1', 'inform', 'storage/audio/inform.mp3', 'storage/images/inform.png'),
    (38, 1, 'B1', 'introduce', 'storage/audio/introduce.mp3', 'storage/images/introduce.png'),
    (39, 1, 'B2', 'convince', 'storage/audio/convince.mp3', 'storage/images/convince.png'),
    (40, 1, 'B2', 'propose', 'storage/audio/propose.mp3', 'storage/images/propose.png'),
    (41, 1, 'B2', 'summarize', 'storage/audio/summarize.mp3', 'storage/images/summarize.png'),
    (42, 1, 'B2', 'acknowledge', 'storage/audio/acknowledge.mp3', 'storage/images/acknowledge.png'),
    (43, 1, 'B2', 'recommend', 'storage/audio/recommend.mp3', 'storage/images/recommend.png'),
    (44, 1, 'B2', 'compromise', 'storage/audio/compromise.mp3', 'storage/images/compromise.png');
