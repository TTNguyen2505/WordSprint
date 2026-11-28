let bridge = null;
let connectionPromise = null;

class QtBridgeError extends Error {
    constructor(message, options) {
        super(message, options);
        this.name = "QtBridgeError";
    }
}

function getBridgeObject(channel) {
    const objects = channel?.objects;
    return (
        objects?.bridgeController ?? objects?.bridge ?? objects?.backend ?? null
    );
}

function connectQtBridge() {
    if (bridge) {
        return Promise.resolve(bridge);
    }
    if (connectionPromise) {
        return connectionPromise;
    }

    const browserWindow = typeof window === "undefined" ? null : window;
    const transport = browserWindow?.qt?.webChannelTransport;

    if (!transport) {
        return Promise.resolve(null);
    }

    if (typeof browserWindow.QWebChannel !== "function") {
        return Promise.reject(
            new QtBridgeError(
                "Qt WebChannel is available, but qwebchannel.js was not loaded."
            )
        );
    }

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
                completed = true;
                clearTimeout(timeoutId);

                if (!backend) {
                    reject(
                        new QtBridgeError(
                            "Qt WebChannel connected, but BridgeController was not registered."
                        )
                    );
                    return;
                }

                bridge = backend;
                resolve(bridge);
            });
        } catch (error) {
            completed = true;
            clearTimeout(timeoutId);
            reject(error);
        }
    }).catch((error) => {
        connectionPromise = null;
        throw error instanceof QtBridgeError
            ? error
            : new QtBridgeError("Could not initialize Qt WebChannel.", {
                  cause: error
              });
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

        return JSON.parse(responseJson);
    } catch (error) {
        throw error instanceof QtBridgeError
            ? error
            : new QtBridgeError(`BridgeController.${methodName} failed.`, {
                  cause: error
              });
    }
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

export const startStage = (request) => invokeDataMethod("startStage", request);
export const completeStage = (request) =>
    invokeBridgeMethod("completeStage", request);
export const getProfiles = () => invokeDataMethod("getProfiles");
export const createProfile = (name) =>
    invokeDataMethod("createProfile", { name });
export const updateAudio = (userId, settings) =>
    invokeDataMethod("updateAudio", { userId, ...settings });
export const getLeaderboard = (stageId) =>
    invokeDataMethod("getLeaderboard", { stageId });
