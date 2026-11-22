import { useEffect, useState } from "react";
import {
    completeStage,
    connectQtBridge,
    getQtBridgeState,
    loadProgress,
    processAnswer,
    processMessage,
    saveProgress,
    startStage,
    subscribeToQtBridge
} from "../qtBridge.js";

export function useQtBridge() {
    const [state, setState] = useState(getQtBridgeState);

    useEffect(() => {
        const unsubscribe = subscribeToQtBridge(setState);
        connectQtBridge().catch(() => {});

        return unsubscribe;
    }, []);

    return {
        ...state,
        isConnected: state.status === "connected",
        isReady: state.status === "connected" && state.backendReady,
        startStage,
        completeStage,
        processAnswer,
        processMessage,
        saveProgress,
        loadProgress
    };
}
