import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Badge from "./Badge";

const statusStyles = {
    Solved: {
        background: "bg-surface-success-subtle",
        badgeType: "Success",
        label: "Solved"
    },
    Active: {
        background: "bg-surface-primary-cta",
        badgeType: "Inverse",
        label: "Current"
    },
    Queued: {
        background: "bg-surface-panel",
        badgeType: "Default",
        label: "Queued"
    }
};

function PipelineItem({
    questionOrdinary = "Question 1",
    status = "Solved",
    state = "Default",
    onClick,
    className
}) {
    const { background, badgeType, label } = statusStyles[status];
    const isQueued = status === "Queued";
    const Element = onClick ? "button" : "div";

    return (
        <Element
            type={onClick ? "button" : undefined}
            onClick={onClick}
            className={twMerge(
                clsx(
                    "flex w-[285px] items-center justify-between gap-xs border-[length:var(--border-width-subtle)] border-stroke-default p-xs text-left text-content-primary shadow-elevation-200",
                    background,
                    isQueued &&
                        "transition-shadow duration-[var(--duration-normal)] ease-snap hover:shadow-elevation-300",
                    isQueued && state === "Hover" && "shadow-elevation-300",
                    onClick && "cursor-pointer",
                    className
                )
            )}
        >
            <span className="text-heading-sm min-w-0 truncate">
                {questionOrdinary}
            </span>
            <Badge type={badgeType} showIcon={false} className="shrink-0">
                {label}
            </Badge>
        </Element>
    );
}

export default PipelineItem;
