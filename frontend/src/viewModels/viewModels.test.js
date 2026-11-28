import assert from "node:assert/strict";
import test from "node:test";
import {
    buildStageQuestions,
    getAudioReplayLimit,
    playQuestionAudio
} from "../audioQuestion.js";
import { createAppSessionStore } from "./appSessionStore.js";
import { createLeaderboardViewModel } from "./leaderboardViewModel.js";

test("stage questions distribute modes evenly and ignore invalid target words", () => {
    const questions = buildStageQuestions(
        [
            { word_id: 1, target_word: "hello" },
            { word_id: 2, target_word: "" },
            { word_id: 3, target_word: "ticket" },
            { word_id: 4, target_word: "screen" },
            { word_id: 5, target_word: "plant" }
        ],
        () => 0
    );
    const modeCounts = questions.reduce((counts, question) => {
        counts[question.mode] = (counts[question.mode] ?? 0) + 1;
        return counts;
    }, {});

    assert.deepEqual(
        questions.map((question) => question.word_id).sort(),
        [1, 3, 4, 5]
    );
    assert.deepEqual(modeCounts, {
        scramble: 2,
        "fill-blank": 1,
        audio: 1
    });
    assert.throws(
        () => buildStageQuestions([{ target_word: "  " }]),
        /no vocabulary/i
    );
    assert.equal(
        buildStageQuestions([{ target_word: "hello" }], () => 0.8)[0].mode,
        "audio"
    );
});

test("audio replay limits match CEFR tiers", () => {
    assert.equal(getAudioReplayLimit("A1"), 3);
    assert.equal(getAudioReplayLimit("A2"), 3);
    assert.equal(getAudioReplayLimit("B1"), 2);
    assert.equal(getAudioReplayLimit("B2"), 2);
    assert.equal(getAudioReplayLimit("C1"), 1);
    assert.equal(getAudioReplayLimit("C2"), 1);
    assert.throws(() => getAudioReplayLimit("D1"), /Unsupported CEFR level/);
});

test("audio playback falls back to the Qt resource after speech synthesis fails", async () => {
    const originals = new Map(
        ["speechSynthesis", "SpeechSynthesisUtterance", "Audio"].map((key) => [
            key,
            Object.getOwnPropertyDescriptor(globalThis, key)
        ])
    );
    const playedUrls = [];

    class MockUtterance {
        constructor(text) {
            this.text = text;
        }
    }
    class MockAudio {
        constructor(url) {
            this.url = url;
            this.listeners = new Map();
            playedUrls.push(url);
        }
        addEventListener(name, callback) {
            this.listeners.set(name, callback);
        }
        removeEventListener(name) {
            this.listeners.delete(name);
        }
        play() {
            setTimeout(() => this.listeners.get("ended")?.(), 0);
            return Promise.resolve();
        }
        pause() {}
    }

    Object.defineProperty(globalThis, "speechSynthesis", {
        configurable: true,
        value: {
            cancel() {},
            speak(utterance) {
                utterance.onerror({ error: "voice-unavailable" });
            }
        }
    });
    Object.defineProperty(globalThis, "SpeechSynthesisUtterance", {
        configurable: true,
        value: MockUtterance
    });
    Object.defineProperty(globalThis, "Audio", {
        configurable: true,
        value: MockAudio
    });

    try {
        const result = await playQuestionAudio({
            targetWord: "hello",
            fallbackUrl: "qrc:/storage/audio/hello.wav",
            fallbackStatus: "available"
        });
        assert.equal(result, "resource");
        assert.deepEqual(playedUrls, ["qrc:/storage/audio/hello.wav"]);
    } finally {
        for (const [key, descriptor] of originals) {
            if (descriptor) {
                Object.defineProperty(globalThis, key, descriptor);
            } else {
                delete globalThis[key];
            }
        }
    }
});

test("audio playback reports missing fallback resources after TTS fails", async () => {
    const originals = new Map(
        ["speechSynthesis", "SpeechSynthesisUtterance"].map((key) => [
            key,
            Object.getOwnPropertyDescriptor(globalThis, key)
        ])
    );
    Object.defineProperty(globalThis, "speechSynthesis", {
        configurable: true,
        value: {
            cancel() {},
            speak: () => {
                throw new Error("Speech failed");
            }
        }
    });
    Object.defineProperty(globalThis, "SpeechSynthesisUtterance", {
        configurable: true,
        value: class MockUtterance {}
    });

    try {
        await assert.rejects(
            playQuestionAudio({
                targetWord: "hello",
                fallbackStatus: "missing"
            }),
            /No fallback pronunciation resource is configured/
        );
    } finally {
        for (const [key, descriptor] of originals) {
            if (descriptor) {
                Object.defineProperty(globalThis, key, descriptor);
            } else {
                delete globalThis[key];
            }
        }
    }
});

test("session uses backend profiles and persists audio for the selected user", async () => {
    let profiles = [
        { id: 1, name: "One" },
        { id: 2, name: "Two" }
    ];
    const saved = [];
    const store = createAppSessionStore({
        getProfiles: async () => ({ profiles }),
        createProfile: async (name) => {
            profiles = [...profiles, { id: 3, name }];
        },
        updateAudio: async (id, settings) => saved.push({ id, settings })
    });
    await store.getState().loadProfiles();
    assert.equal(store.getState().activeId, 1);
    store.getState().switchProfile(2);
    await store.getState().createProfile("Three");
    assert.equal(store.getState().activeId, 2);
    assert.equal(store.getState().profiles.length, 3);
    await store.getState().updateAudio({ musicVolume: 42 });
    assert.deepEqual(saved, [{ id: 2, settings: { musicVolume: 42 } }]);
    assert.equal(store.getState().profiles[1].musicVolume, 42);
    assert.throws(() => store.getState().switchProfile(99), /not found/);
});

test("session exposes failures without updating saved audio", async () => {
    const store = createAppSessionStore({
        getProfiles: async () => ({ profiles: [{ id: 1, musicVolume: 50 }] }),
        updateAudio: async () => {
            throw new Error("Save failed");
        }
    });
    await store.getState().loadProfiles();
    await assert.rejects(
        store.getState().updateAudio({ musicVolume: 10 }),
        /Save failed/
    );
    assert.equal(store.getState().profiles[0].musicVolume, 50);

    const failed = createAppSessionStore({
        getProfiles: async () => {
            throw new Error("Unavailable");
        }
    });
    await assert.rejects(failed.getState().loadProfiles(), /Unavailable/);
    assert.equal(failed.getState().status, "error");
    assert.equal(failed.getState().error, "Unavailable");
});

test("leaderboard ignores stale responses and exposes errors", async () => {
    const pending = new Map();
    const store = createLeaderboardViewModel(
        (stageId) =>
            new Promise((resolve, reject) => {
                pending.set(stageId, { resolve, reject });
            })
    );
    const first = store.getState().selectTab("overall");
    const second = store.getState().selectTab(5);
    pending.get(5).resolve({ records: [{ rank: 1 }] });
    await second;
    pending.get(null).resolve({ records: [{ rank: 2 }] });
    await first;
    assert.equal(store.getState().activeTab, 5);
    assert.equal(store.getState().records[0].rank, 1);
    const third = store.getState().selectTab(6);
    pending.get(6).reject(new Error("Could not load"));
    await third;
    assert.equal(store.getState().status, "error");
    assert.equal(store.getState().error, "Could not load");
});
