import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import SuccessIcon from "../assets/icons/success.svg?react";

function ProgressBar({
    value = 0,
    max = 100,
    status,
    statusLabel,
    metricLabel,
    showCompletedIcon = true,
    className
}) {
    const clampedValue = Math.min(Math.max(value, 0), max);
    const isCompleted = status === "completed" || clampedValue >= max;
    const percent = max === 0 ? 0 : (clampedValue / max) * 100;
    const leftLabel =
        statusLabel ??
        (isCompleted ? "COMPLETED STATUS LABEL" : "IN PROGRESS STATUS LABEL");
    const rightLabel =
        metricLabel ??
        (isCompleted ? "SUCCESS METRIC LABEL" : "IN PROGRESS METRIC LABEL");
    const fillColor = isCompleted
        ? "bg-surface-success"
        : "bg-surface-primary-cta";

    return (
        <div
            className={twMerge(
                clsx("flex w-full max-w-[480px] flex-col gap-2xs", className)
            )}
        >
            <div className="flex w-full items-center justify-between">
                <div className="flex items-center gap-2xs">
                    {isCompleted && showCompletedIcon && (
                        <span className="flex size-icon-xs items-center justify-center text-content-primary">
                            <SuccessIcon
                                className="size-full text-current"
                                aria-hidden="true"
                            />
                        </span>
                    )}
                    <span className="text-mono-badge text-content-primary">
                        {leftLabel}
                    </span>
                </div>
                <span className="text-mono-badge text-content-primary">
                    {rightLabel}
                </span>
            </div>

            <div
                role="progressbar"
                aria-valuenow={clampedValue}
                aria-valuemin={0}
                aria-valuemax={max}
                className="w-full overflow-hidden rounded-none border-[length:var(--border-width-subtle)] border-stroke-default bg-surface-panel p-3xs"
            >
                <div
                    className={twMerge(
                        clsx(
                            "h-[var(--primitive-space-8)] border-[length:var(--border-width-subtle)] border-stroke-default transition-[width] duration-[var(--duration-normal)] ease-snap",
                            fillColor
                        )
                    )}
                    style={{ width: `${percent}%` }}
                />
            </div>
        </div>
    );
}

export default ProgressBar;
