import assert from "node:assert/strict";
import test from "node:test";
import { createAppSessionStore } from "./appSessionStore.js";
import { createLeaderboardViewModel } from "./leaderboardViewModel.js";

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
