import { useRef, useState } from "react";
import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import ArrowBackIcon from "@/assets/icons/arrow-back-1.svg?react";
import ArrowForwardIcon from "@/assets/icons/arrow-forward-1.svg?react";
import MusicNoteIcon from "@/assets/icons/music_note.svg?react";
import Badge from "@/components/primitives/Badge";
import Button from "@/components/primitives/Button";
import QuizLetter from "./QuizLetter";
import TextInput from "@/components/primitives/TextInput";

const DEFAULT_LETTERS = {
    scramble: ["U", "Q", "S", "E", "T", "O", "I", "N"],
    "fill-blank": ["Q", "U", "", "S", "T", "", "O", "N"],
    audio: Array(8).fill("")
};

function Quiz({
    mode = "scramble",
    badgeLabel,
    imageSrc,
    imageAlt = "Quiz clue illustration",
    onImageError,
    letters,
    answer,
    answerError = "",
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
    answerDisabled = false,
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
    const inputRef = useRef(null);
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
        if (onSubmit?.((answer ?? internalAnswer).trim()) === false) {
            inputRef.current?.select();
        }
    };

    const currentAnswer = answer ?? internalAnswer;
    const isPlayDisabled = disabled || playsRemaining <= 0;
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

            <div className="aspect-[16/9] w-full max-w-[480px] overflow-hidden border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel shadow-elevation-200">
                <img
                    src={imageSrc}
                    alt={imageAlt}
                    onError={onImageError}
                    className="size-full object-cover"
                />
            </div>

            {mode === "audio" && (
                <Button
                    label={`PLAY SOUND (${playsRemaining}/${maxPlays})`}
                    variant="primary"
                    size="large"
                    leadingIcon={<MusicNoteIcon className="size-full" />}
                    showTrailingIcon={false}
                    onClick={onPlaySound}
                    disabled={isPlayDisabled || answerDisabled}
                />
            )}

            <div className="flex w-full flex-wrap items-start justify-center gap-sm">
                {effectiveLetters.map((letter, index) =>
                    letter === " " ? (
                        <span key={index} className="w-4" aria-label="Space" />
                    ) : (
                        <QuizLetter
                            key={`${letter}-${index}`}
                            letter={letter}
                            blank={String(letter).trim().length === 0}
                            disabled={disabled || answerDisabled}
                        />
                    )
                )}
            </div>

            <form
                onSubmit={handleSubmit}
                className="flex w-[480px] flex-col gap-md"
            >
                <TextInput
                    className="w-full"
                    value={currentAnswer}
                    inputRef={inputRef}
                    onChange={handleInputChange}
                    placeholder="Enter your answer here"
                    showTitle={false}
                    state={answerError ? "error" : "default"}
                    showSupportingText={Boolean(answerError)}
                    supportingText={answerError}
                    showLeadingIcon={false}
                    aria-label="Enter your answer here"
                    disabled={disabled || answerDisabled}
                />
                <div className="flex w-full items-center justify-end gap-sm">
                    <Button
                        label="Clear"
                        variant="secondary"
                        size="default"
                        showLeadingIcon={false}
                        showTrailingIcon={false}
                        onClick={handleClear}
                        disabled={disabled || answerDisabled}
                        type="button"
                    />
                    <Button
                        label="Submit"
                        variant="primary"
                        size="default"
                        showLeadingIcon={false}
                        showTrailingIcon={false}
                        disabled={disabled || answerDisabled}
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
