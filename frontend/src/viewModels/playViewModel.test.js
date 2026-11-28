import assert from "node:assert/strict";
import test from "node:test";
import { createPlayViewModel, findQueuedQuestion } from "./playViewModel.js";

function fixture(words, options = {}) {
    let time = 0;
    const records = [];
    const api = {
        startStage: async (request) => {
            assert.deepEqual(request, { userId: 2, stageId: 1 });
            return {
                status: "ok",
                cefrLevel: options.level ?? "A1",
                timeLimit: 60,
                passScore: options.passScore ?? 30,
                words
            };
        },
        completeStage: async (record) => {
            records.push(record);
            return { status: "ok", nextStageId: 2, nextStageUnlocked: true };
        }
    };
    const store = createPlayViewModel({
        stageId: 1,
        userId: 2,
        api,
        now: () => time,
        random: () => 0.999
    });
    return {
        store,
        records,
        advance(ms) {
            time += ms;
            store.getState().tick();
        }
    };
}

function words(count, audioIndices = []) {
    return Array.from({ length: count }, (_, index) => ({
        target_word: `word${index}`,
        image_path: `storage/images/word${index}.png`,
        audio_path: audioIndices.includes(index)
            ? `storage/audio/word${index}.mp3`
            : null
    }));
}

test("generates balanced modes, reserves audio words and masks A1 correctly", async () => {
    const game = fixture(words(9, [0, 3, 6]));
    await game.store.getState().start();
    const questions = game.store.getState().questions;
    assert.deepEqual(
        ["scramble", "fill-blank", "audio"].map(
            (mode) => questions.filter((q) => q.mode === mode).length
        ),
        [3, 3, 3]
    );
    for (const question of questions) {
        if (question.mode === "audio") assert.ok(question.audio_path);
        if (question.mode === "fill-blank") {
            assert.equal(question.letters[0], "w");
            assert.equal(
                question.letters.filter((letter) => letter === "").length,
                1
            );
        }
        if (question.mode === "scramble") {
            assert.notEqual(question.letters.join(""), question.target_word);
        }
    }
});

test("scores streaks, resets on mistakes and saves once on completion", async () => {
    const game = fixture(words(4));
    await game.store.getState().start();
    game.advance(3000);
    for (let index = 0; index < 3; index++) {
        game.store.getState().updateAnswer(` WORD${index} `);
        game.store.getState().submitAnswer();
    }
    assert.equal(game.store.getState().score, 32);
    game.store.getState().updateAnswer("incorrect");
    game.store.getState().submitAnswer();
    assert.equal(game.store.getState().streak, 0);
    assert.match(game.store.getState().answerError, /Incorrect/);
    game.store.getState().updateAnswer("word3");
    assert.equal(game.store.getState().questions[3].answer, "WORD3");
    assert.equal(game.store.getState().answerError, "");
    game.store.getState().submitAnswer();
    await Promise.resolve();
    assert.equal(game.store.getState().status, "complete");
    assert.equal(game.store.getState().result.passed, true);
    assert.deepEqual(game.records, [
        { userId: 2, stageId: 1, score: 42, completionTime: 0 }
    ]);
    game.store.getState().submitAnswer();
    game.advance(60000);
    assert.equal(game.records.length, 1);
});

test("audio questions mask every letter and preserve spaces", async () => {
    const items = words(3, [0]);
    items[0].target_word = "ice cream";
    const game = fixture(items);
    await game.store.getState().start();
    const audio = game.store.getState().questions[0];
    assert.equal(audio.mode, "audio");
    assert.deepEqual(audio.letters, ["", "", "", " ", "", "", "", "", ""]);
});

test("queued navigation finds the nearest unsolved question without wrapping", () => {
    const questions = [
        { solved: false },
        { solved: true },
        { solved: false },
        { solved: true },
        { solved: false }
    ];
    assert.equal(findQueuedQuestion(questions, 2, -1), 0);
    assert.equal(findQueuedQuestion(questions, 2, 1), 4);
    assert.equal(findQueuedQuestion(questions, 0, -1), -1);
    assert.equal(findQueuedQuestion(questions, 4, 1), -1);
    assert.equal(findQueuedQuestion(questions, 1, 1), 2);
    questions[4].solved = true;
    assert.equal(findQueuedQuestion(questions, 2, 1), -1);
});

test("answer feedback clears when navigating or editing", async () => {
    const game = fixture(words(2));
    await game.store.getState().start();
    game.advance(3000);
    game.store.getState().updateAnswer("wrong");
    assert.equal(game.store.getState().questions[0].answer, "WRONG");
    game.store.getState().submitAnswer();
    assert.match(game.store.getState().answerError, /Incorrect/);
    game.store.getState().updateAnswer("w");
    assert.equal(game.store.getState().answerError, "");
    game.store.getState().submitAnswer();
    game.store.getState().selectQuestion(1);
    assert.equal(game.store.getState().answerError, "");
    game.store.getState().selectQuestion(0);
    assert.equal(game.store.getState().questions[0].answer, "W");
    assert.equal(game.store.getState().answerError, "");
});

test("pausing and resume countdown do not consume play time; timeout saves once", async () => {
    const game = fixture(words(2));
    await game.store.getState().start();
    game.advance(3000);
    game.advance(1250);
    game.store.getState().pause();
    game.advance(10000);
    assert.equal(game.store.getState().remaining, 59);
    game.store.getState().resume();
    game.advance(3000);
    assert.equal(game.store.getState().remaining, 59);
    game.advance(58750);
    await Promise.resolve();
    assert.equal(game.store.getState().result.reason, "timeUp");
    assert.equal(game.store.getState().result.passed, false);
    assert.equal(game.records[0].completionTime, 60);
    game.advance(1000);
    assert.equal(game.records.length, 1);
});

test("audio auto-play is separate from replay limits and skipping resets streak", async () => {
    const game = fixture(words(3, [0]), { level: "B1" });
    await game.store.getState().start();
    game.advance(3000);
    assert.equal(game.store.getState().questions[0].mode, "audio");
    assert.ok(game.store.getState().takeAudioPlay(true));
    assert.equal(game.store.getState().takeAudioPlay(true), null);
    assert.equal(game.store.getState().questions[0].playsRemaining, 2);
    assert.ok(game.store.getState().takeAudioPlay());
    assert.ok(game.store.getState().takeAudioPlay());
    assert.equal(game.store.getState().takeAudioPlay(), null);
    game.store.getState().updateAnswer("word0");
    game.store.getState().submitAnswer();
    assert.equal(game.store.getState().streak, 1);
    game.store.getState().selectQuestion(2);
    assert.equal(game.store.getState().streak, 0);
});

test("fill blanks and audio replay limits follow each CEFR difficulty", async () => {
    for (const [level, hidden, plays] of [
        ["A1", 2, 3],
        ["A2", 2, 3],
        ["B1", 3, 2],
        ["B2", 3, 2],
        ["C1", 4, 1],
        ["C2", 4, 1]
    ]) {
        const items = words(3, [0]);
        items.forEach((word) => {
            word.target_word = "planet";
        });
        const game = fixture(items, { level });
        await game.store.getState().start();
        const questions = game.store.getState().questions;
        const fill = questions.find(
            (question) => question.mode === "fill-blank"
        );
        const audio = questions.find((question) => question.mode === "audio");
        assert.equal(
            fill.letters.filter((letter) => letter === "").length,
            hidden,
            level
        );
        assert.equal(audio.playsRemaining, plays, level);
        game.advance(3000);
        assert.ok(game.store.getState().takeAudioPlay(true), level);
        assert.equal(game.store.getState().takeAudioPlay(true), null, level);
        assert.equal(
            game.store.getState().questions[0].playsRemaining,
            plays,
            level
        );
        for (let replay = 0; replay < plays; replay++) {
            assert.ok(game.store.getState().takeAudioPlay(), level);
        }
        assert.equal(game.store.getState().takeAudioPlay(), null, level);
        if (level === "A1" || level === "A2")
            assert.equal(fill.letters[0], "p");
    }
});

test("surfaces start and save errors instead of claiming success", async () => {
    const failing = createPlayViewModel({
        stageId: 1,
        userId: 2,
        api: {
            startStage: async () => ({
                status: "error",
                message: "Unavailable"
            })
        }
    });
    await failing.getState().start();
    assert.equal(failing.getState().status, "error");
    assert.match(failing.getState().error, /Unavailable/);

    let time = 0;
    const store = createPlayViewModel({
        stageId: 1,
        userId: 2,
        now: () => time,
        api: {
            startStage: async () => ({
                status: "ok",
                cefrLevel: "A1",
                timeLimit: 1,
                passScore: 10,
                words: words(1)
            }),
            completeStage: async () => ({
                status: "error",
                message: "Storage failed"
            })
        }
    });
    await store.getState().start();
    time = 4000;
    store.getState().tick();
    await Promise.resolve();
    assert.equal(store.getState().status, "saveError");
    assert.match(store.getState().error, /Storage failed/);
});

test("start retries after synchronous failures and expiration rejects late answers", async () => {
    let attempts = 0;
    let time = 0;
    let saved;
    const store = createPlayViewModel({
        stageId: 1,
        userId: 2,
        now: () => time,
        api: {
            startStage: () => {
                if (++attempts === 1) throw new Error("Disconnected");
                return {
                    status: "ok",
                    cefrLevel: "A1",
                    timeLimit: 1,
                    passScore: 10,
                    words: words(1)
                };
            },
            completeStage: async (record) => {
                saved = record;
                return { status: "ok" };
            }
        }
    });
    await store.getState().start();
    assert.equal(store.getState().status, "error");
    await store.getState().start();
    time = 3000;
    store.getState().tick();
    store.getState().updateAnswer("word0");
    time = 4000;
    store.getState().submitAnswer();
    await Promise.resolve();
    assert.equal(saved.score, 0);
    assert.equal(store.getState().result.reason, "timeUp");
});
