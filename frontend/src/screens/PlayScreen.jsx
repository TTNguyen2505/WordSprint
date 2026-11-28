import { useCallback, useEffect, useRef, useState } from "react";
import { useStore } from "zustand";
import ClockIcon from "../assets/icons/clock.svg?react";
import PauseIcon from "../assets/icons/pause.svg?react";
import TopicIcon from "../assets/icons/topic.svg?react";
import Badge from "../components/primitives/Badge";
import Button from "../components/primitives/Button";
import InfoPill from "../components/primitives/InfoPill";
import PipelineItem from "../components/composite/PipelineItem";
import Quiz from "../components/composite/Quiz/Quiz";
import PauseModal from "../modals/PauseModal";
import {
    createPlayViewModel,
    findQueuedQuestion
} from "../viewModels/playViewModel";
import { formatTime } from "../utils/formatTime";

function PlayScreen({
    username = "Player_1",
    stageId,
    userId,
    topic = "SCHOOL",
    level = "A1",
    initialMusicEnabled = true,
    initialMusicVolume = 100,
    initialSoundEnabled = true,
    initialSoundVolume = 100,
    onAudioSettingsChange,
    onSaveAudioSettings,
    onComplete,
    onReplay,
    onQuit
}) {
    const [store] = useState(() => createPlayViewModel({ stageId, userId }));
    const state = useStore(store);
    const audioRef = useRef(null);
    const reportedResult = useRef(false);
    const question = state.questions[state.activeQuestion];
    const previousQueued = findQueuedQuestion(
        state.questions,
        state.activeQuestion,
        -1
    );
    const nextQueued = findQueuedQuestion(
        state.questions,
        state.activeQuestion,
        1
    );
    const paused = state.status === "paused";
    const time = formatTime(state.remaining);

    useEffect(() => {
        store.getState().start();
        const timer = window.setInterval(() => store.getState().tick(), 200);
        return () => window.clearInterval(timer);
    }, [store]);

    useEffect(() => {
        if (state.status === "complete" && !reportedResult.current) {
            reportedResult.current = true;
            onComplete?.(state.result);
        }
    }, [state.status, state.result, onComplete]);

    const playAudio = useCallback(
        (path) => {
            if (!path) return;
            audioRef.current?.pause();
            const audio = new Audio(`qrc:///${path}`);
            audioRef.current = audio;
            audio.play().catch(() => {
                if (audioRef.current === audio)
                    store.getState().reportAudioError();
            });
        },
        [store]
    );

    useEffect(() => {
        audioRef.current?.pause();
        audioRef.current = null;
        const timer =
            state.status === "playing"
                ? window.setTimeout(
                      () => playAudio(store.getState().takeAudioPlay(true)),
                      0
                  )
                : null;
        return () => {
            if (timer !== null) window.clearTimeout(timer);
            audioRef.current?.pause();
            audioRef.current = null;
        };
    }, [store, playAudio, state.status, state.activeQuestion]);

    if (
        state.status === "idle" ||
        state.status === "loading" ||
        state.status === "error" ||
        state.status === "saveError"
    ) {
        return (
            <main className="flex h-[900px] w-[1200px] flex-col items-center justify-center gap-lg bg-surface-canvas">
                <p role={state.error ? "alert" : "status"}>
                    {state.error || "Loading stage..."}
                </p>
                {state.status === "error" && (
                    <Button
                        label="Retry"
                        showLeadingIcon={false}
                        showTrailingIcon={false}
                        onClick={() => store.getState().start()}
                    />
                )}
                <Button
                    label="Quit to menu"
                    showLeadingIcon={false}
                    showTrailingIcon={false}
                    onClick={onQuit}
                />
            </main>
        );
    }

    return (
        <>
            {state.status === "countdown" ? (
                <main className="flex h-[900px] w-[1200px] items-center justify-center bg-surface-canvas text-content-primary">
                    <p
                        role="status"
                        aria-atomic="true"
                        className="text-display-hero"
                    >
                        <span className="sr-only">Starting in </span>
                        {state.countdown}
                    </p>
                </main>
            ) : (
                <div inert={paused}>
                    <main className="flex h-[900px] w-[1200px] bg-surface-canvas text-content-primary">
                        <aside className="flex h-full w-[360px] shrink-0 flex-col bg-surface-panel">
                            <header className="border-[length:var(--border-width-heavy)] border-stroke-default px-md py-sm">
                                <Button
                                    label="PAUSE"
                                    size="default"
                                    variant="secondary"
                                    className="w-full"
                                    leadingIcon={
                                        <PauseIcon className="size-full" />
                                    }
                                    showTrailingIcon={false}
                                    onClick={state.pause}
                                />
                            </header>
                            <section
                                aria-label="Questions"
                                className="flex min-h-0 flex-1 flex-col gap-lg overflow-y-auto border-x-[length:var(--border-width-heavy)] border-b-[length:var(--border-width-heavy)] border-stroke-default px-md pt-2xl pb-sm"
                            >
                                <h2 className="text-heading-sm">
                                    QUESTIONS ({state.solved}/
                                    {state.questions.length})
                                </h2>
                                <div className="flex flex-col gap-md">
                                    {Array.from(
                                        { length: state.questions.length },
                                        (_, index) => (
                                            <PipelineItem
                                                key={index}
                                                questionOrdinary={`Question ${index + 1}`}
                                                status={
                                                    state.questions[index]
                                                        .solved
                                                        ? "Solved"
                                                        : index ===
                                                            state.activeQuestion
                                                          ? "Active"
                                                          : "Queued"
                                                }
                                                className="w-full"
                                                onClick={
                                                    paused
                                                        ? undefined
                                                        : () =>
                                                              state.selectQuestion(
                                                                  index
                                                              )
                                                }
                                            />
                                        )
                                    )}
                                </div>
                            </section>
                            <footer className="flex flex-col gap-sm border-x-[length:var(--border-width-heavy)] border-b-[length:var(--border-width-heavy)] border-stroke-default px-md py-sm">
                                <div className="flex items-center justify-between gap-xs">
                                    <span className="text-heading-sm">
                                        CURRENT PLAYER:
                                    </span>
                                    <Badge type="Highlight" showIcon={false}>
                                        {username}
                                    </Badge>
                                </div>
                                <div className="border-t-[length:var(--border-width-default)] border-stroke-default" />
                                <div className="flex items-center justify-between gap-xs">
                                    <span className="text-heading-sm">
                                        ACCUMULATED SCORE:
                                    </span>
                                    <Badge type="Highlight" showIcon={false}>
                                        {state.score}
                                    </Badge>
                                </div>
                            </footer>
                        </aside>

                        <section
                            aria-label="Quiz"
                            className="flex h-full w-[840px] flex-col items-center justify-between overflow-hidden px-2xl pt-xl"
                        >
                            <header className="flex w-full items-center justify-between">
                                <div className="flex items-center gap-[10px]">
                                    <InfoPill
                                        label="Topic"
                                        value={topic}
                                        leadingIcon={
                                            <TopicIcon className="size-full" />
                                        }
                                        showTrailingIcon={false}
                                    />
                                    <InfoPill
                                        label="Level"
                                        value={level}
                                        showLeadingIcon={false}
                                        showTrailingIcon={false}
                                    />
                                </div>
                                <InfoPill
                                    label="Time"
                                    value={time}
                                    leadingIcon={
                                        <ClockIcon className="size-full" />
                                    }
                                    showTrailingIcon={false}
                                />
                            </header>
                            <div className="relative w-full">
                                <Quiz
                                    className="max-w-none"
                                    mode={question.mode}
                                    imageSrc={`qrc:///${question.image_path}`}
                                    onImageError={state.reportImageError}
                                    letters={question.letters}
                                    answer={question.answer}
                                    answerError={state.answerError}
                                    onChange={state.updateAnswer}
                                    onSubmit={state.submitAnswer}
                                    answerDisabled={question.solved}
                                    onPrevious={() =>
                                        state.selectQuestion(previousQueued)
                                    }
                                    onNext={() =>
                                        state.selectQuestion(nextQueued)
                                    }
                                    hasPrevious={previousQueued !== -1}
                                    hasNext={nextQueued !== -1}
                                    playsRemaining={question.playsRemaining}
                                    maxPlays={
                                        {
                                            A1: 3,
                                            A2: 3,
                                            B1: 2,
                                            B2: 2,
                                            C1: 1,
                                            C2: 1
                                        }[state.level]
                                    }
                                    onPlaySound={() =>
                                        playAudio(state.takeAudioPlay())
                                    }
                                    disabled={state.status !== "playing"}
                                />
                                {state.message && (
                                    <p
                                        role="status"
                                        className="text-body-default absolute top-full mt-xs w-full text-center"
                                    >
                                        {state.message}
                                    </p>
                                )}
                                {state.status === "saving" && (
                                    <p
                                        role="status"
                                        className="text-body-default absolute top-full mt-xs w-full text-center"
                                    >
                                        Saving result...
                                    </p>
                                )}
                            </div>
                            <div />
                        </section>
                    </main>
                </div>
            )}
            <PauseModal
                isOpen={paused}
                solved={state.solved}
                total={state.questions.length}
                timeLeft={time}
                score={state.score}
                initialMusicEnabled={initialMusicEnabled}
                initialMusicVolume={initialMusicVolume}
                initialSoundEnabled={initialSoundEnabled}
                initialSoundVolume={initialSoundVolume}
                onAudioSettingsChange={onAudioSettingsChange}
                onSaveSettings={onSaveAudioSettings}
                onResume={state.resume}
                onReplay={onReplay}
                onQuit={onQuit}
            />
        </>
    );
}

export default PlayScreen;
