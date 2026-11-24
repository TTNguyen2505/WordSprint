import { create } from "zustand";
import { getLeaderboard } from "../qtBridge.js";

export function createLeaderboardViewModel(loadLeaderboard = getLeaderboard) {
    let requestId = 0;
    return create((set) => ({
        activeTab: "overall",
        records: [],
        status: "idle",
        error: null,
        async selectTab(stageId) {
            const currentRequest = ++requestId;
            set({
                activeTab: stageId,
                records: [],
                status: "loading",
                error: null
            });
            try {
                const { records } = await loadLeaderboard(
                    stageId === "overall" ? null : stageId
                );
                if (currentRequest === requestId) {
                    set({ records, status: "ready" });
                }
            } catch (error) {
                if (currentRequest === requestId) {
                    set({ status: "error", error: error.message });
                }
            }
        }
    }));
}

export const useLeaderboardViewModel = createLeaderboardViewModel();
