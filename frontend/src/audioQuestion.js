const QUESTION_MODES = ["scramble", "fill-blank", "audio"];

export function getAudioReplayLimit(cefrLevel) {
    if (cefrLevel === "A1" || cefrLevel === "A2") return 3;
    if (cefrLevel === "B1" || cefrLevel === "B2") return 2;
    if (cefrLevel === "C1" || cefrLevel === "C2") return 1;
    throw new Error(`Unsupported CEFR level for audio playback: ${cefrLevel}`);
}

export function buildStageQuestions(words, random = Math.random) {
    const validWords = words.filter(
        (word) =>
            typeof word.target_word === "string" &&
            word.target_word.trim().length > 0
    );
    if (validWords.length === 0) {
        throw new Error(
            "The stage has no vocabulary with a valid target word."
        );
    }

    const firstMode = Math.floor(random() * QUESTION_MODES.length);
    const modes = validWords.map(
        (_, index) =>
            QUESTION_MODES[(index + firstMode) % QUESTION_MODES.length]
    );
    for (let index = modes.length - 1; index > 0; index -= 1) {
        const swapIndex = Math.floor(random() * (index + 1));
        [modes[index], modes[swapIndex]] = [modes[swapIndex], modes[index]];
    }

    return validWords.map((word, index) => ({ ...word, mode: modes[index] }));
}

function createAbortError() {
    const error = new Error("Audio playback was cancelled.");
    error.name = "AbortError";
    return error;
}

function throwIfAborted(signal) {
    if (signal?.aborted) throw createAbortError();
}

function speakWithBrowser(text, signal) {
    return new Promise((resolve, reject) => {
        const synthesis = globalThis.speechSynthesis;
        const Utterance = globalThis.SpeechSynthesisUtterance;
        if (
            !synthesis ||
            typeof synthesis.speak !== "function" ||
            typeof Utterance !== "function"
        ) {
            reject(new Error("Speech synthesis is unavailable."));
            return;
        }

        let settled = false;
        let timeoutId;
        const utterance = new Utterance(text);
        utterance.lang = "en-US";
        const finish = (callback, value) => {
            if (settled) return;
            settled = true;
            clearTimeout(timeoutId);
            signal?.removeEventListener("abort", onAbort);
            callback(value);
        };
        const onAbort = () => {
            synthesis.cancel();
            finish(reject, createAbortError());
        };
        if (signal?.aborted) {
            reject(createAbortError());
            return;
        }

        utterance.onend = () => finish(resolve);
        utterance.onerror = (event) =>
            finish(
                reject,
                new Error(
                    event.error || "Speech synthesis could not play the word."
                )
            );
        signal?.addEventListener("abort", onAbort, { once: true });
        timeoutId = setTimeout(() => {
            synthesis.cancel();
            finish(reject, new Error("Speech synthesis timed out."));
        }, 15000);

        try {
            synthesis.cancel();
            synthesis.speak(utterance);
        } catch (error) {
            finish(reject, error);
        }
    });
}

function playFallbackResource(url, signal) {
    return new Promise((resolve, reject) => {
        if (typeof globalThis.Audio !== "function") {
            reject(new Error("Audio playback is unavailable."));
            return;
        }

        let settled = false;
        let timeoutId;
        const audio = new globalThis.Audio(url);
        const cleanup = () => {
            clearTimeout(timeoutId);
            audio.removeEventListener("ended", onEnded);
            audio.removeEventListener("error", onError);
            signal?.removeEventListener("abort", onAbort);
        };
        const finish = (callback, value) => {
            if (settled) return;
            settled = true;
            cleanup();
            callback(value);
        };
        const onEnded = () => finish(resolve);
        const onError = () =>
            finish(
                reject,
                new Error(
                    `The pronunciation audio resource "${url}" could not be played.`
                )
            );
        const onAbort = () => {
            audio.pause();
            finish(reject, createAbortError());
        };

        if (signal?.aborted) {
            reject(createAbortError());
            return;
        }
        audio.addEventListener("ended", onEnded);
        audio.addEventListener("error", onError);
        signal?.addEventListener("abort", onAbort, { once: true });
        timeoutId = setTimeout(() => {
            audio.pause();
            finish(
                reject,
                new Error(
                    "The pronunciation audio resource playback timed out."
                )
            );
        }, 30000);

        try {
            const playback = audio.play();
            playback?.catch(onError);
        } catch (error) {
            finish(reject, error);
        }
    });
}

function fallbackStatusMessage(status) {
    switch (status) {
        case "invalid_path":
            return "The configured audio_path is invalid.";
        case "not_found":
            return "The configured pronunciation resource was not found in Qt resources.";
        default:
            return "No fallback pronunciation resource is configured.";
    }
}

export async function playQuestionAudio({
    targetWord,
    fallbackUrl,
    fallbackStatus,
    signal
}) {
    const word = typeof targetWord === "string" ? targetWord.trim() : "";
    if (!word) {
        throw new Error("This audio question has no valid target word.");
    }
    throwIfAborted(signal);

    let speechError;
    try {
        await speakWithBrowser(word, signal);
        return "speech";
    } catch (error) {
        if (signal?.aborted) throw createAbortError();
        speechError = error;
    }

    if (!fallbackUrl) {
        throw new Error(
            `Could not pronounce "${word}" (${speechError.message}). ${fallbackStatusMessage(fallbackStatus)}`
        );
    }

    try {
        await playFallbackResource(fallbackUrl, signal);
        return "resource";
    } catch (error) {
        if (signal?.aborted) throw createAbortError();
        throw new Error(
            `Speech synthesis failed (${speechError.message}), and the fallback pronunciation resource failed: ${error.message}`,
            { cause: error }
        );
    }
}
