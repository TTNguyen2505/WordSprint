import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import { useState } from "react";
import Badge from "./Badge";
import QuizLetter from "./QuizLetter";

/**
 * Neo‑Brutalism Quiz component.
 * Supports three modes: scramble, fill‑blank, audio.
 * Implements badge, navigation, illustration, optional audio button, letter tiles,
 * answer input, clear/submit actions and an instruction prompt.
 */
function Quiz({
    mode = "scramble",
    badgeLabel,
    imageSrc,
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
    onPlaySound,
    playsRemaining = 3,
    maxPlays = 3,
    instruction,
    disabled = false,
    className
}) {
    // ----- Helpers -----
    const modeLabels = {
        scramble: "SCRAMBLE",
        "fill-blank": "FILL BLANK",
        audio: "AUDIO"
    };

    const currentBadgeLabel = badgeLabel ?? modeLabels[mode] ?? "SCRAMBLE";

    // ----- Answer state (controlled / uncontrolled) -----
    const [internalAnswer, setInternalAnswer] = useState(answer ?? "");

    const currentAnswer = answer !== undefined ? answer : internalAnswer;

    const handleInputChange = (e) => {
        const val = e.target.value;
        if (answer === undefined) {
            setInternalAnswer(val);
        }
        if (typeof onChange === "function") {
            onChange(val);
        }
    };

    const handleClear = () => {
        if (answer === undefined) {
            setInternalAnswer("");
        }
        if (typeof onClear === "function") {
            onClear();
        }
        if (typeof onChange === "function") {
            onChange("");
        }
    };

    const handleSubmit = (e) => {
        e.preventDefault();
        const trimmed = currentAnswer.trim();
        if (typeof onSubmit === "function") {
            onSubmit(trimmed);
        }
    };

    // ----- Letters handling -----
    const defaultLetterCount = 8;
    const effectiveLetters =
        letters ?? (mode === "audio" ? Array(defaultLetterCount).fill("") : []);

    // ----- Instruction default per mode -----
    const defaultInstructions = {
        scramble: "Rearrange all the letters above into the correct word.",
        "fill-blank": "Type the full word, including the missing letters.",
        audio: "Listen carefully and type the word you hear."
    };

    const currentInstruction = instruction ?? defaultInstructions[mode] ?? "";

    // ----- Button disabled logic -----
    const isPrevDisabled = disabled || !hasPrevious;
    const isNextDisabled = disabled || !hasNext;
    const isPlayDisabled = disabled || playsRemaining <= 0;
    const isSubmitDisabled = disabled;
    const isClearDisabled = disabled;

    return (
        <div
            className={twMerge(
                clsx(
                    "w-full max-w-[580px] bg-surface-panel border border-stroke-default rounded-none",
                    "shadow-elevation-200 p-6 flex flex-col gap-5",
                    className
                )
            )}
        >
            {/* Header row */}
            <div className="flex items-center justify-between gap-2">
                {/* Mode badge */}
                <Badge variant="pink" showIcon={false} className="px-2 py-1">
                    {currentBadgeLabel}
                </Badge>
                {/* Navigation */}
                <div className="flex items-center gap-2">
                    <button
                        type="button"
                        onClick={onPrevious}
                        disabled={isPrevDisabled}
                        className={twMerge(
                            clsx(
                                "inline-flex items-center gap-1 px-3 py-2 border border-stroke-default rounded-none",
                                "bg-surface-panel text-content-primary",
                                "shadow-elevation-200",
                                !isPrevDisabled &&
                                    "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] active:translate-x-[2px] active:translate-y-[2px] transition-all",
                                isPrevDisabled &&
                                    "opacity-50 cursor-not-allowed"
                            )
                        )}
                    >
                        <span aria-hidden="true">&lt;</span> Previous
                    </button>
                    <button
                        type="button"
                        onClick={onNext}
                        disabled={isNextDisabled}
                        className={twMerge(
                            clsx(
                                "inline-flex items-center gap-1 px-3 py-2 border border-stroke-default rounded-none",
                                "bg-surface-panel text-content-primary",
                                "shadow-elevation-200",
                                !isNextDisabled &&
                                    "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] active:translate-x-[2px] active:translate-y-[2px] transition-all",
                                isNextDisabled &&
                                    "opacity-50 cursor-not-allowed"
                            )
                        )}
                    >
                        Next <span aria-hidden="true">&gt;</span>
                    </button>
                </div>
            </div>

            {/* Illustration frame */}
            <div className="w-full aspect-[16/9] border border-stroke-default rounded-none shadow-elevation-200 overflow-hidden bg-surface-panel flex items-center justify-center">
                {imageSrc ? (
                    <img
                        src={imageSrc}
                        alt={imageAlt}
                        className="w-full h-full object-cover"
                    />
                ) : (
                    <span className="text-content-muted">No image</span>
                )}
            </div>

            {/* Audio button – only for audio mode */}
            {mode === "audio" && (
                <div className="flex justify-center">
                    <button
                        type="button"
                        onClick={onPlaySound}
                        disabled={isPlayDisabled}
                        className={twMerge(
                            clsx(
                                "inline-flex items-center gap-2 px-6 py-2.5 border border-stroke-default rounded-none",
                                "bg-surface-primary-cta text-content-primary font-mono font-bold uppercase text-xs",
                                "shadow-elevation-200",
                                !isPlayDisabled &&
                                    "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] active:translate-x-[2px] active:translate-y-[2px] transition-all",
                                isPlayDisabled &&
                                    "opacity-50 cursor-not-allowed"
                            )
                        )}
                    >
                        <span aria-hidden="true">♪</span> PLAY SOUND (
                        {playsRemaining}/{maxPlays})
                    </button>
                </div>
            )}

            {/* Letter tiles row */}
            <div className="flex flex-wrap items-center justify-center gap-2">
                {effectiveLetters.map((ltr, idx) => (
                    <QuizLetter
                        key={idx}
                        letter={ltr}
                        // For interactive modes you might want an onClick, but spec does not require it.
                    />
                ))}
            </div>

            {/* Answer input and action buttons */}
            <form onSubmit={handleSubmit} className="flex flex-col gap-4">
                <input
                    type="text"
                    value={currentAnswer}
                    onChange={handleInputChange}
                    placeholder="Enter your answer here"
                    disabled={disabled}
                    className={twMerge(
                        clsx(
                            "w-full bg-surface-panel border border-stroke-default rounded-none",
                            "shadow-elevation-200 px-3.5 py-2.5",
                            "text-sm font-mono text-content-primary placeholder:text-neutral-400",
                            "focus:outline-none focus:bg-[#FFFDF0]",
                            disabled && "opacity-50 cursor-not-allowed"
                        )
                    )}
                />
                <div className="flex items-center justify-end gap-2.5">
                    <button
                        type="button"
                        onClick={handleClear}
                        disabled={isClearDisabled}
                        className={twMerge(
                            clsx(
                                "px-5 py-2 text-xs font-bold border border-stroke-default rounded-none",
                                "bg-surface-panel text-content-primary",
                                "shadow-elevation-200",
                                !isClearDisabled &&
                                    "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] active:translate-x-[2px] active:translate-y-[2px] transition-all",
                                isClearDisabled &&
                                    "opacity-50 cursor-not-allowed"
                            )
                        )}
                    >
                        Clear
                    </button>
                    <button
                        type="submit"
                        disabled={isSubmitDisabled}
                        className={twMerge(
                            clsx(
                                "px-5 py-2 text-xs font-bold border border-stroke-default rounded-none",
                                "bg-surface-primary-cta text-content-primary",
                                "shadow-elevation-200",
                                !isSubmitDisabled &&
                                    "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] active:translate-x-[2px] active:translate-y-[2px] transition-all",
                                isSubmitDisabled &&
                                    "opacity-50 cursor-not-allowed"
                            )
                        )}
                    >
                        Submit
                    </button>
                </div>
            </form>

            {/* Instruction helper */}
            <p className="text-center text-xs text-neutral-700 font-medium">
                {currentInstruction}
            </p>
        </div>
    );
}

export default Quiz;
