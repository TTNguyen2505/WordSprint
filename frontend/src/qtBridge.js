const bridgeListeners = new Set();
const signalDisconnectors = [];

let bridge = null;
let connectionPromise = null;
let currentState = {
    status: "disconnected",
    backendReady: false,
    error: null,
    lastEvent: null
};

export class QtBridgeError extends Error {
    constructor(message, options) {
        super(message, options);
        this.name = "QtBridgeError";
    }
}

export function getQtBridgeState() {
    return currentState;
}

function updateState(nextValues) {
    currentState = { ...currentState, ...nextValues };
    bridgeListeners.forEach((listener) => listener(currentState));
}

function publishEvent(event) {
    updateState({ lastEvent: event });
}

function disconnectSignals() {
    signalDisconnectors.splice(0).forEach((disconnect) => disconnect());
}

function connectSignal(signal, callback, name) {
    if (
        !signal ||
        typeof signal.connect !== "function" ||
        typeof signal.disconnect !== "function"
    ) {
        throw new QtBridgeError(
            `Qt WebChannel signal "${name}" is unavailable.`
        );
    }

    signal.connect(callback);
    signalDisconnectors.push(() => signal.disconnect(callback));
}

function attachSignals() {
    if (
        !bridge ||
        signalDisconnectors.length > 0 ||
        bridgeListeners.size === 0
    ) {
        return;
    }

    try {
        connectSignal(
            bridge.stageStarted,
            (stageId) => publishEvent({ type: "stageStarted", stageId }),
            "stageStarted"
        );
        connectSignal(
            bridge.stageCompleted,
            (stageId, score) =>
                publishEvent({ type: "stageCompleted", stageId, score }),
            "stageCompleted"
        );
        connectSignal(
            bridge.jsonMessage,
            (payload) => {
                let data = payload;
                try {
                    data = JSON.parse(payload);
                } catch (error) {
                    updateState({
                        error: new QtBridgeError(
                            "The backend sent an invalid JSON message.",
                            { cause: error }
                        )
                    });
                }

                publishEvent({ type: "jsonMessage", payload, data });
            },
            "jsonMessage"
        );
        connectSignal(
            bridge.isReadyChanged,
            (backendReady) => {
                updateState({ backendReady: Boolean(backendReady) });
                publishEvent({
                    type: "isReadyChanged",
                    backendReady: Boolean(backendReady)
                });
            },
            "isReadyChanged"
        );
    } catch (error) {
        disconnectSignals();
        throw error;
    }
}

export function subscribeToQtBridge(listener) {
    bridgeListeners.add(listener);
    attachSignals();

    return () => {
        bridgeListeners.delete(listener);
        if (bridgeListeners.size === 0) {
            disconnectSignals();
        }
    };
}

function getBridgeObject(channel) {
    const objects = channel?.objects;
    return (
        objects?.bridgeController ?? objects?.bridge ?? objects?.backend ?? null
    );
}

export function connectQtBridge() {
    if (bridge) {
        return Promise.resolve(bridge);
    }
    if (connectionPromise) {
        return connectionPromise;
    }

    const browserWindow = typeof window === "undefined" ? null : window;
    const transport = browserWindow?.qt?.webChannelTransport;

    if (!transport) {
        updateState({
            status: "unavailable",
            backendReady: false,
            error: null
        });
        return Promise.resolve(null);
    }

    if (typeof browserWindow.QWebChannel !== "function") {
        const error = new QtBridgeError(
            "Qt WebChannel is available, but qwebchannel.js was not loaded."
        );
        updateState({ status: "error", backendReady: false, error });
        connectionPromise = Promise.reject(error);
        return connectionPromise;
    }

    updateState({ status: "connecting", backendReady: false, error: null });

    connectionPromise = new Promise((resolve, reject) => {
        let completed = false;
        const timeoutId = setTimeout(() => {
            if (completed) {
                return;
            }
            completed = true;
            reject(
                new QtBridgeError(
                    "Timed out while connecting to Qt WebChannel."
                )
            );
        }, 10000);

        try {
            new browserWindow.QWebChannel(transport, (channel) => {
                if (completed) {
                    return;
                }

                const backend = getBridgeObject(channel);
                if (!backend) {
                    completed = true;
                    clearTimeout(timeoutId);
                    reject(
                        new QtBridgeError(
                            "Qt WebChannel connected, but BridgeController was not registered."
                        )
                    );
                    return;
                }

                try {
                    bridge = backend;
                    attachSignals();
                    completed = true;
                    clearTimeout(timeoutId);
                    updateState({
                        status: "connected",
                        backendReady: Boolean(bridge.isReady),
                        error: null
                    });
                    resolve(bridge);
                } catch (error) {
                    bridge = null;
                    completed = true;
                    clearTimeout(timeoutId);
                    reject(error);
                }
            });
        } catch (error) {
            completed = true;
            clearTimeout(timeoutId);
            reject(error);
        }
    }).catch((error) => {
        const bridgeError =
            error instanceof QtBridgeError
                ? error
                : new QtBridgeError("Could not initialize Qt WebChannel.", {
                      cause: error
                  });
        updateState({
            status: "error",
            backendReady: false,
            error: bridgeError
        });
        throw bridgeError;
    });

    return connectionPromise;
}

async function invokeBridgeMethod(methodName, request = {}) {
    try {
        const backend = await connectQtBridge();
        if (!backend) {
            throw new QtBridgeError(
                "Qt WebChannel is unavailable outside the Qt desktop application."
            );
        }

        if (typeof backend[methodName] !== "function") {
            throw new QtBridgeError(
                `BridgeController does not expose the "${methodName}" method.`
            );
        }

        const requestJson =
            typeof request === "string"
                ? request
                : JSON.stringify(request ?? {});
        const responseJson = await backend[methodName](requestJson);

        if (typeof responseJson !== "string") {
            throw new QtBridgeError(
                `BridgeController.${methodName} did not return a JSON string.`
            );
        }

        const response = JSON.parse(responseJson);
        updateState({ error: null });
        return response;
    } catch (error) {
        const bridgeError =
            error instanceof QtBridgeError
                ? error
                : new QtBridgeError(`BridgeController.${methodName} failed.`, {
                      cause: error
                  });
        updateState({ error: bridgeError });
        throw bridgeError;
    }
}

export function startStage(request) {
    return invokeBridgeMethod("startStage", request);
}

export function completeStage(request) {
    return invokeBridgeMethod("completeStage", request);
}

export function processAnswer(request) {
    return invokeBridgeMethod("processAnswer", request);
}

export function processMessage(request) {
    return invokeBridgeMethod("processMessage", request);
}

export function saveProgress(request) {
    return invokeBridgeMethod("saveProgress", request);
}

export function loadProgress(request) {
    return invokeBridgeMethod("loadProgress", request);
}

async function invokeDataMethod(methodName, request = {}) {
    const response = await invokeBridgeMethod(methodName, request);
    if (response.status !== "ok") {
        throw new QtBridgeError(
            response.message || `BridgeController.${methodName} failed.`
        );
    }
    return response;
}

export const getProfiles = () => invokeDataMethod("getProfiles");
export const createProfile = (name) =>
    invokeDataMethod("createProfile", { name });
export const updateAudio = (userId, settings) =>
    invokeDataMethod("updateAudio", { userId, ...settings });
export const getLeaderboard = (stageId) =>
    invokeDataMethod("getLeaderboard", { stageId });
