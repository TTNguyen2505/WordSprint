import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import TrophyIcon from "@/assets/icons/trophy.svg?react";
import ArrowForwardIcon from "@/assets/icons/arrow-forward-1.svg?react";
import Button from "@/components/primitives/Button";

function ScoreCard({
    label = "TOTAL SCORE",
    score = 124850,
    unit = "PTS",
    buttonLabel = "Leaderboard",
    onAction,
    icon,
    state = "Default",
    className
}) {
    const formattedScore =
        typeof score === "number" ? score.toLocaleString() : score;
    const isHover = state === "Hover" || state === "hover";

    return (
        <div
            onClick={onAction}
            className={twMerge(
                clsx(
                    "inline-flex w-fit max-w-full items-center gap-lg rounded-none border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel p-md shadow-elevation-200 transition-[translate,box-shadow] duration-[var(--duration-normal)] ease-snap hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] hover:shadow-elevation-300",
                    onAction && "cursor-pointer",
                    isHover && "shadow-elevation-300",
                    className
                )
            )}
        >
            <div className="flex size-[var(--primitive-space-48)] shrink-0 items-center justify-center rounded-none border-[length:var(--border-width-default)] border-stroke-default bg-surface-primary-cta p-xs shadow-elevation-100">
                {icon ?? (
                    <TrophyIcon
                        className="size-icon-md text-content-primary"
                        aria-hidden="true"
                    />
                )}
            </div>
            <div className="flex shrink-0 flex-col gap-xs text-content-primary">
                <span className="text-mono-badge">{label}</span>
                <span className="flex items-center gap-xs text-mono-data">
                    <span>{formattedScore}</span>
                    <span>{unit}</span>
                </span>
            </div>
            <Button
                label={buttonLabel}
                variant="inverse"
                size="small"
                showLeadingIcon={false}
                trailingIcon={<ArrowForwardIcon className="size-full" />}
            />
        </div>
    );
}

export default ScoreCard;
