import { useCallback, useEffect, useRef, useState } from "react";
import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import { playQuestionAudio } from "../audioQuestion.js";
import thumbnail from "../assets/quiz-question-thumbnail.png";
import ArrowBackIcon from "../assets/icons/arrow_back_1.svg?react";
import ArrowForwardIcon from "../assets/icons/arrow_forward_1.svg?react";
import MusicNoteIcon from "../assets/icons/music_note.svg?react";
import Badge from "./Badge";
import Button from "./Button";
import QuizLetter from "./QuizLetter";
import TextInput from "./TextInput";

const DEFAULT_LETTERS = {
    scramble: ["U", "Q", "S", "E", "T", "O", "I", "N"],
    "fill-blank": ["Q", "U", "", "S", "T", "", "O", "N"],
    audio: Array(8).fill("")
};

function Quiz({
    mode = "scramble",
    badgeLabel,
    imageSrc = thumbnail,
    imageAlt = "Quiz clue illustration",
    letters,
    answer,
    onChange,
    onSubmit,
    onClear,
    onPrevious,
    hasPrevious = true,
    onNext,
    hasNext = true,
    audioQuestionId,
    audioText,
    audioFallbackUrl,
    audioFallbackStatus,
    maxPlays = 3,
    instruction,
    disabled = false,
    className
}) {
    const modeLabels = {
        scramble: "SCRAMBLE",
        "fill-blank": "FILL BLANK",
        audio: "AUDIO"
    };
    const currentBadgeLabel =
        badgeLabel ?? modeLabels[mode] ?? modeLabels.scramble;
    const [internalAnswer, setInternalAnswer] = useState(answer ?? "");
    const [playsRemaining, setPlaysRemaining] = useState(maxPlays);
    const [audioStatus, setAudioStatus] = useState("idle");
    const [audioError, setAudioError] = useState("");
    const remainingPlaysRef = useRef(maxPlays);
    const activePlaybackRef = useRef(null);
    const effectiveLetters = letters ?? DEFAULT_LETTERS[mode] ?? [];
    const currentInstruction =
        instruction ??
        {
            scramble: "Rearrange all the letters above into the correct word.",
            "fill-blank": "Type the full word, including the missing letters.",
            audio: "Listen carefully and type the word you hear."
        }[mode] ??
        "";

    const handleInputChange = (event) => {
        const nextAnswer = event.target.value;
        if (answer === undefined) setInternalAnswer(nextAnswer);
        onChange?.(nextAnswer);
    };

    const handleClear = () => {
        if (answer === undefined) setInternalAnswer("");
        onClear?.();
        onChange?.("");
    };

    const handleSubmit = (event) => {
        event.preventDefault();
        onSubmit?.((answer ?? internalAnswer).trim());
    };

    const currentAnswer = answer ?? internalAnswer;
    const playAudio = useCallback(
        async (manual = false) => {
            if (activePlaybackRef.current) return;
            if (manual && remainingPlaysRef.current <= 0) return;

            const controller = new AbortController();
            activePlaybackRef.current = controller;
            setAudioStatus("playing");
            setAudioError("");

            try {
                await playQuestionAudio({
                    targetWord: audioText,
                    fallbackUrl: audioFallbackUrl,
                    fallbackStatus: audioFallbackStatus,
                    signal: controller.signal
                });
                if (controller.signal.aborted) return;
                if (manual) {
                    remainingPlaysRef.current -= 1;
                    setPlaysRemaining(remainingPlaysRef.current);
                }
                setAudioStatus("idle");
            } catch (error) {
                if (!controller.signal.aborted) {
                    setAudioError(error.message);
                    setAudioStatus("error");
                }
            } finally {
                if (activePlaybackRef.current === controller) {
                    activePlaybackRef.current = null;
                }
            }
        },
        [audioFallbackStatus, audioFallbackUrl, audioText]
    );

    useEffect(() => {
        if (mode !== "audio") return undefined;

        const timeoutId = window.setTimeout(() => {
            void playAudio();
        }, 0);
        return () => {
            window.clearTimeout(timeoutId);
            activePlaybackRef.current?.abort();
            activePlaybackRef.current = null;
        };
    }, [audioQuestionId, mode, playAudio]);

    const isPlayDisabled =
        disabled || playsRemaining <= 0 || audioStatus === "playing";
    return (
        <section
            className={twMerge(
                clsx(
                    "flex w-full max-w-[548px] flex-col items-center border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel px-xl py-lg shadow-elevation-200",
                    "gap-xl",
                    className
                )
            )}
        >
            <header className="flex w-full items-center justify-between">
                <Badge type="Highlight" showIcon={false}>
                    {currentBadgeLabel}
                </Badge>
                <nav
                    aria-label="Quiz navigation"
                    className="flex items-center gap-sm"
                >
                    <Button
                        label="Previous"
                        variant="secondary"
                        size="default"
                        leadingIcon={<ArrowBackIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={onPrevious}
                        disabled={disabled || !hasPrevious}
                    />
                    <Button
                        label="Next"
                        variant="secondary"
                        size="default"
                        showLeadingIcon={false}
                        trailingIcon={
                            <ArrowForwardIcon className="size-full" />
                        }
                        onClick={onNext}
                        disabled={disabled || !hasNext}
                    />
                </nav>
            </header>

            {mode !== "audio" && (
                <div className="aspect-[16/9] w-full max-w-[480px] overflow-hidden border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel shadow-elevation-200">
                    <img
                        src={imageSrc}
                        alt={imageAlt}
                        className="size-full object-cover"
                    />
                </div>
            )}

            {mode === "audio" && (
                <>
                    <Button
                        label={`PLAY SOUND (${playsRemaining}/${maxPlays})`}
                        variant="primary"
                        size="large"
                        leadingIcon={<MusicNoteIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={() => void playAudio(true)}
                        disabled={isPlayDisabled}
                    />
                    {audioStatus === "playing" && (
                        <p role="status">Playing pronunciation...</p>
                    )}
                    {audioError && (
                        <p
                            role="alert"
                            className="text-body-default text-center"
                        >
                            {audioError}
                        </p>
                    )}
                </>
            )}

            <div className="flex w-full flex-wrap items-start justify-center gap-sm">
                {effectiveLetters.map((letter, index) => (
                    <QuizLetter
                        key={`${letter}-${index}`}
                        letter={letter}
                        blank={String(letter).trim().length === 0}
                        disabled={disabled}
                    />
                ))}
            </div>

            <form
                onSubmit={handleSubmit}
                className="flex w-full max-w-[480px] flex-col gap-md"
            >
                <TextInput
                    className="w-full"
                    value={currentAnswer}
                    onChange={handleInputChange}
                    placeholder="Enter your answer here"
                    showTitle={false}
                    showSupportingText={false}
                    showLeadingIcon={false}
                    aria-label="Enter your answer here"
                    disabled={disabled}
                />
                <div className="flex w-full items-center justify-end gap-sm">
                    <Button
                        label="Clear"
                        variant="secondary"
                        size="default"
                        showLeadingIcon={false}
                        showTrailingIcon={false}
                        onClick={handleClear}
                        disabled={disabled}
                        type="button"
                    />
                    <Button
                        label="Submit"
                        variant="primary"
                        size="default"
                        showLeadingIcon={false}
                        showTrailingIcon={false}
                        disabled={disabled}
                        type="submit"
                    />
                </div>
            </form>

            <p className="text-body-default text-center text-content-primary">
                {currentInstruction}
            </p>
        </section>
    );
}

export default Quiz;
