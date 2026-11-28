import { createStore } from "zustand/vanilla";
import { completeStage, startStage } from "../services/qtBridge.js";

const defaultApi = { startStage, completeStage };

const levels = {
    A1: { points: 10, hidden: 0.35, plays: 3 },
    A2: { points: 15, hidden: 0.35, plays: 3 },
    B1: { points: 20, hidden: 0.5, plays: 2 },
    B2: { points: 25, hidden: 0.5, plays: 2 },
    C1: { points: 30, hidden: 0.65, plays: 1 },
    C2: { points: 35, hidden: 0.65, plays: 1 }
};

function shuffle(items, random) {
    const result = [...items];
    for (let i = result.length - 1; i > 0; i--) {
        const j = Math.floor(random() * (i + 1));
        [result[i], result[j]] = [result[j], result[i]];
    }
    return result;
}

function scramble(word, random) {
    return word.replace(/[a-z]+/gi, (part) => {
        const letters = [...part];
        let shuffled = shuffle(letters, random);
        if (shuffled.join("") === part) {
            const different = letters.findIndex(
                (letter) => letter !== letters[0]
            );
            if (different > 0) {
                shuffled = [...letters];
                [shuffled[0], shuffled[different]] = [
                    shuffled[different],
                    shuffled[0]
                ];
            }
        }
        return shuffled.join("");
    });
}

function makeQuestions(words, level, random) {
    const rules = levels[level];
    const bag = Array.from(
        { length: words.length },
        (_, index) => ["scramble", "fill-blank", "audio"][index % 3]
    );
    const extras = words.length % 3;
    if (extras) {
        const choices = shuffle(["scramble", "fill-blank", "audio"], random);
        for (let i = 0; i < extras; i++)
            bag[words.length - extras + i] = choices[i];
    }
    const audioIndices = shuffle(
        words.flatMap((word, index) => (word.audio_path ? [index] : [])),
        random
    );
    const audioCount = Math.min(
        audioIndices.length,
        bag.filter((mode) => mode === "audio").length
    );
    const modes = Array(words.length).fill(null);
    audioIndices.slice(0, audioCount).forEach((index) => {
        modes[index] = "audio";
    });
    const nonAudio = bag.filter((mode) => mode !== "audio");
    while (nonAudio.length < words.length - audioCount) {
        const scrambleCount = nonAudio.filter(
            (mode) => mode === "scramble"
        ).length;
        nonAudio.push(
            scrambleCount <= nonAudio.length - scrambleCount
                ? "scramble"
                : "fill-blank"
        );
    }
    const shuffledModes = shuffle(nonAudio, random);
    return words.map((word, index) => {
        const mode = modes[index] ?? shuffledModes.pop();
        const chars = [...word.target_word];
        let letters;
        if (mode === "scramble") {
            letters = [...scramble(word.target_word, random)];
        } else if (mode === "fill-blank") {
            const indices = chars.flatMap((char, index) =>
                /[a-z]/i.test(char) ? [index] : []
            );
            const hidden = Math.min(
                indices.length - 1,
                Math.max(1, Math.round(indices.length * rules.hidden))
            );
            const eligible =
                level === "A1" || level === "A2" ? indices.slice(1) : indices;
            const masked = new Set(
                shuffle(eligible, random).slice(0, Math.max(0, hidden))
            );
            letters = chars.map((char, index) =>
                masked.has(index) ? "" : char
            );
        } else {
            letters = chars.map((char) => (/[a-z]/i.test(char) ? "" : char));
        }
        return {
            ...word,
            mode,
            letters,
            answer: "",
            solved: false,
            playsRemaining: rules.plays,
            autoPlayed: false
        };
    });
}

export function findQueuedQuestion(questions, activeIndex, direction) {
    for (
        let index = activeIndex + direction;
        index >= 0 && index < questions.length;
        index += direction
    ) {
        if (!questions[index].solved) return index;
    }
    return -1;
}

export function createPlayViewModel({
    stageId,
    userId,
    api = defaultApi,
    now = () => performance.now(),
    random = Math.random
}) {
    let startRequest;
    let deadline = 0;
    let countdownEnd = 0;
    let remainingMs = 0;

    return createStore((set, get) => {
        const finish = async (reason) => {
            const state = get();
            if (state.status !== "playing") return;
            const completionTime =
                state.timeLimit - Math.ceil(remainingMs / 1000);
            set({ status: "saving", remaining: Math.ceil(remainingMs / 1000) });
            try {
                const response = await api.completeStage({
                    userId,
                    stageId,
                    score: state.score,
                    completionTime
                });
                if (response.status !== "ok")
                    throw new Error(
                        response.message || "Could not save the stage result."
                    );
                set({
                    status: "complete",
                    result: {
                        reason,
                        score: state.score,
                        solved: state.solved,
                        total: state.questions.length,
                        completionTime,
                        passed: state.score >= state.passScore,
                        passScore: state.passScore,
                        nextStageId: response.nextStageId,
                        nextStageUnlocked: response.nextStageUnlocked
                    }
                });
            } catch (error) {
                set({
                    status: "saveError",
                    error: `Could not confirm the result was saved: ${error.message}. Check the leaderboard before replaying.`
                });
            }
        };
        const countdown = () => {
            countdownEnd = now() + 3000;
            set({ status: "countdown", countdown: 3, message: "" });
        };
        return {
            status: "idle",
            error: null,
            questions: [],
            activeQuestion: 0,
            score: 0,
            streak: 0,
            solved: 0,
            remaining: 0,
            countdown: 3,
            timeLimit: 0,
            passScore: 0,
            level: null,
            message: "",
            answerError: "",
            result: null,
            start() {
                if (startRequest) return startRequest;
                set({ status: "loading", error: null });
                startRequest = (async () => {
                    try {
                        const response = await Promise.resolve().then(() =>
                            api.startStage({ userId, stageId })
                        );
                        if (response.status !== "ok")
                            throw new Error(
                                response.message || "Could not start the stage."
                            );
                        if (
                            !levels[response.cefrLevel] ||
                            !Number.isInteger(response.timeLimit) ||
                            response.timeLimit <= 0 ||
                            !Number.isInteger(response.passScore) ||
                            response.passScore < 0 ||
                            !Array.isArray(response.words) ||
                            response.words.length === 0 ||
                            response.words.some(
                                (word) =>
                                    typeof word.target_word !== "string" ||
                                    !word.target_word.trim() ||
                                    typeof word.image_path !== "string" ||
                                    !word.image_path
                            )
                        )
                            throw new Error("The stage data is invalid.");
                        const questions = makeQuestions(
                            response.words,
                            response.cefrLevel,
                            random
                        );
                        remainingMs = response.timeLimit * 1000;
                        set({
                            questions,
                            timeLimit: response.timeLimit,
                            remaining: response.timeLimit,
                            passScore: response.passScore,
                            level: response.cefrLevel
                        });
                        countdown();
                    } catch (error) {
                        set({ status: "error", error: error.message });
                        startRequest = null;
                    }
                })();
                return startRequest;
            },
            tick() {
                const state = get();
                if (state.status === "countdown") {
                    const left = countdownEnd - now();
                    if (left > 0) {
                        set({ countdown: Math.ceil(left / 1000) });
                        return;
                    }
                    deadline = countdownEnd + remainingMs;
                    set({ status: "playing", countdown: 0 });
                }
                if (get().status !== "playing") return;
                remainingMs = Math.max(0, deadline - now());
                set({ remaining: Math.ceil(remainingMs / 1000) });
                if (remainingMs === 0) void finish("timeUp");
            },
            pause() {
                get().tick();
                if (get().status !== "playing") return;
                set({ status: "paused" });
            },
            resume() {
                if (get().status === "paused") countdown();
            },
            selectQuestion(index) {
                get().tick();
                const state = get();
                if (
                    state.status !== "playing" ||
                    !Number.isInteger(index) ||
                    index < 0 ||
                    index >= state.questions.length ||
                    index === state.activeQuestion
                )
                    return;
                set({
                    activeQuestion: index,
                    message: "",
                    answerError: "",
                    streak: state.questions[state.activeQuestion].solved
                        ? state.streak
                        : 0
                });
            },
            updateAnswer(answer) {
                const state = get();
                if (
                    state.status !== "playing" ||
                    state.questions[state.activeQuestion].solved
                )
                    return;
                set({
                    questions: state.questions.map((question, index) =>
                        index === state.activeQuestion
                            ? { ...question, answer: answer.toUpperCase() }
                            : question
                    ),
                    message: "",
                    answerError: ""
                });
            },
            submitAnswer() {
                get().tick();
                const state = get();
                const current = state.questions[state.activeQuestion];
                if (state.status !== "playing" || current.solved) return;
                if (
                    current.answer.trim().toLowerCase() !==
                    current.target_word.trim().toLowerCase()
                ) {
                    set({
                        streak: 0,
                        answerError: "Incorrect answer. Try again."
                    });
                    return false;
                }
                const streak = state.streak + 1;
                const questions = state.questions.map((question, index) =>
                    index === state.activeQuestion
                        ? { ...question, solved: true }
                        : question
                );
                const next = questions.findIndex(
                    (question, index) =>
                        index > state.activeQuestion && !question.solved
                );
                const first = questions.findIndex(
                    (question) => !question.solved
                );
                set({
                    questions,
                    streak,
                    solved: state.solved + 1,
                    score:
                        state.score +
                        levels[state.level].points +
                        (streak >= 3 ? 2 : 0),
                    activeQuestion:
                        next >= 0
                            ? next
                            : first >= 0
                              ? first
                              : state.activeQuestion,
                    message: "",
                    answerError: ""
                });
                if (first < 0) void finish("solved");
                return true;
            },
            takeAudioPlay(automatic = false) {
                get().tick();
                const state = get();
                const question = state.questions[state.activeQuestion];
                if (
                    state.status !== "playing" ||
                    question.mode !== "audio" ||
                    question.solved ||
                    (automatic
                        ? question.autoPlayed
                        : question.playsRemaining <= 0)
                )
                    return null;
                set({
                    questions: state.questions.map((item, index) =>
                        index === state.activeQuestion
                            ? {
                                  ...item,
                                  autoPlayed: automatic
                                      ? true
                                      : item.autoPlayed,
                                  playsRemaining: automatic
                                      ? item.playsRemaining
                                      : item.playsRemaining - 1
                              }
                            : item
                    )
                });
                return question.audio_path;
            },
            reportAudioError() {
                set({ message: "Could not play the pronunciation audio." });
            },
            reportImageError() {
                set({ message: "Could not load the question image." });
            }
        };
    });
}
