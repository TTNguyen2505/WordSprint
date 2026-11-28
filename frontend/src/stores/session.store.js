import { create } from "zustand";
import {
    createProfile,
    getProfiles,
    updateAudio
} from "../services/qtBridge.js";

const defaultApi = { getProfiles, createProfile, updateAudio };

export function createAppSessionStore(api = defaultApi) {
    let loadingProfiles = null;
    return create((set, get) => ({
        profiles: [],
        activeId: null,
        status: "idle",
        error: null,
        loadProfiles() {
            if (loadingProfiles) return loadingProfiles;
            if (get().status !== "ready")
                set({ status: "loading", error: null });
            loadingProfiles = (async () => {
                try {
                    const { profiles } = await Promise.resolve().then(() =>
                        api.getProfiles()
                    );
                    const activeId = profiles.some(
                        (profile) => profile.id === get().activeId
                    )
                        ? get().activeId
                        : (profiles[0]?.id ?? null);
                    set({ profiles, activeId, status: "ready", error: null });
                } catch (error) {
                    set({ status: "error", error: error.message });
                    throw error;
                } finally {
                    loadingProfiles = null;
                }
            })();
            return loadingProfiles;
        },
        switchProfile(userId) {
            if (!get().profiles.some((profile) => profile.id === userId)) {
                throw new Error("Profile was not found.");
            }
            set({ activeId: userId });
        },
        async createProfile(name) {
            await api.createProfile(name);
            try {
                await get().loadProfiles();
            } catch (error) {
                throw new Error(
                    "Profile was created, but profiles could not be refreshed.",
                    { cause: error }
                );
            }
        },
        async updateAudio(settings) {
            const userId = get().activeId;
            if (userId === null) throw new Error("No active profile.");
            await api.updateAudio(userId, settings);
            set((state) => ({
                profiles: state.profiles.map((profile) =>
                    profile.id === userId
                        ? { ...profile, ...settings }
                        : profile
                )
            }));
        }
    }));
}

export const useAppSessionStore = createAppSessionStore();

const selectActiveProfile = (state) =>
    state.profiles.find((profile) => profile.id === state.activeId) ?? null;

export function useActiveProfile() {
    return useAppSessionStore(selectActiveProfile);
}
