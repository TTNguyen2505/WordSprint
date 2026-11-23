import clsx from "clsx";
import { twMerge } from "tailwind-merge";

function QuizLetter({
    letter = "L",
    blank = false,
    onClick,
    disabled = false,
    className
}) {
    const isDisabled = disabled;
    const displayedLetter = String(letter).trim().charAt(0).toUpperCase();
    const isBlank = blank || displayedLetter.length === 0;

    const baseClasses = clsx(
        "inline-flex size-[var(--primitive-space-48)] shrink-0 items-center justify-center rounded-none border-[length:var(--border-width-default)] border-stroke-default",
        "bg-surface-panel",
        "shadow-elevation-200",
        !isDisabled &&
            onClick &&
            "cursor-pointer transition-[translate,box-shadow] duration-[var(--duration-fast)] ease-snap hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] active:translate-x-[var(--primitive-depth-100)] active:translate-y-[var(--primitive-depth-100)]",
        isDisabled && "cursor-not-allowed opacity-50",
        className
    );

    const handleClick = (e) => {
        if (!isDisabled && typeof onClick === "function") {
            onClick(e);
        }
    };

    return (
        <div
            role="button"
            onClick={handleClick}
            className={twMerge(baseClasses)}
        >
            {isBlank ? (
                <span className="text-heading-md select-none text-content-primary">
                    _
                </span>
            ) : (
                <span className="text-heading-md select-none text-content-primary">
                    {displayedLetter}
                </span>
            )}
        </div>
    );
}

export default QuizLetter;
