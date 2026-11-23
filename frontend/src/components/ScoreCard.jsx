import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import TrophyIcon from "../assets/icons/trophy.svg?react";

/**
 * Neo‑Brutalism ScoreCard component.
 * Displays a trophy badge, a score label/value, and a right‑aligned action button.
 *
 * Props:
 *   - label (string) – label above the score (default: "TOTAL SCORE").
 *   - score (number|string) – numeric score (default: 124850). Numbers are formatted with commas.
 *   - unit (string) – unit displayed after the score (default: "PTS").
 *   - buttonLabel (string) – text for the right‑hand button (default: "Leaderboard").
 *   - onAction (function) – click handler for the action button.
 *   - icon (ReactNode) – optional custom trophy icon; falls back to the default trophy.
 *   - className (string) – optional extra Tailwind classes for the outer card.
 */
function ScoreCard({
    label = "TOTAL SCORE",
    score = 124850,
    unit = "PTS",
    buttonLabel = "Leaderboard",
    onAction,
    icon,
    className
}) {
    // Format numeric score with commas when a number is supplied.
    const formattedScore =
        typeof score === "number" ? score.toLocaleString() : score;

    const handleClick = (e) => {
        if (typeof onAction === "function") {
            onAction(e);
        }
    };

    return (
        <div
            className={twMerge(
                clsx(
                    "flex items-center justify-between gap-3 rounded-none border-2 border-stroke-default",
                    "shadow-elevation-200 bg-surface-panel p-md",
                    "max-w-[350px] w-full",
                    className
                )
            )}
        >
            {/* Left side: trophy badge + text */}
            <div className="flex items-center gap-3">
                {/* Trophy badge */}
                <div className="flex shrink-0 items-center justify-center w-11 h-11 rounded-none border-2 border-stroke-default bg-surface-primary-cta shadow-elevation-200">
                    {icon ?? (
                        <TrophyIcon
                            className="size-[20px] text-content-primary"
                            aria-hidden="true"
                        />
                    )}
                </div>
                {/* Text stack */}
                <div className="flex flex-col gap-0.5">
                    <span className="text-[11px] font-mono font-bold uppercase tracking-wider text-content-primary">
                        {label}
                    </span>
                    <span className="text-sm font-mono font-semibold text-content-primary whitespace-nowrap">
                        {formattedScore} {unit}
                    </span>
                </div>
            </div>

            {/* Right side: action button */}
            <button
                type="button"
                onClick={handleClick}
                className={twMerge(
                    clsx(
                        "inline-flex items-center gap-1.5 shrink-0 bg-surface-inverse text-content-inverse text-xs font-medium",
                        "px-3 py-2 rounded-none",
                        "cursor-pointer hover:bg-neutral-800 active:translate-x-[1px] active:translate-y-[1px] transition-all"
                    )
                )}
            >
                {buttonLabel}
                {/* Simple chevron using a Unicode character */}
                <span aria-hidden="true">&gt;</span>
            </button>
        </div>
    );
}

export default ScoreCard;
