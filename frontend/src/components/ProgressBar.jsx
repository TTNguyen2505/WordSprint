import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import CheckIcon from "../assets/icons/check.svg?react";

/**
 * Neo‑Brutalism ProgressBar component.
 *
 * Props:
 *   - value (number) – current progress, default 0.
 *   - max (number) – maximum value, default 100.
 *   - status ("in-progress" | "completed") – optional explicit status; if omitted the component infers completion when value >= max.
 *   - statusLabel (string) – label displayed on the left; defaults depend on the inferred/computed status.
 *   - metricLabel (string) – optional right‑aligned metric label.
 *   - showCompletedIcon (boolean) – show a black check‑mark when completed, default true.
 *   - className (string) – optional extra Tailwind classes for the outer wrapper.
 */
function ProgressBar({
  value = 0,
  max = 100,
  status,
  statusLabel,
  metricLabel,
  showCompletedIcon = true,
  className,
}) {
  const clampedValue = Math.min(Math.max(value, 0), max);
  const isCompleted = status === "completed" || clampedValue >= max;
  const effectiveStatus = isCompleted ? "completed" : "in-progress";

  // Determine labels when not provided.
  const defaultStatusLabel = isCompleted
    ? "COMPLETED STATUS LABEL"
    : "IN PROGRESS STATUS LABEL";
  const leftLabel = statusLabel ?? defaultStatusLabel;

  // Width percentage for fill bar.
  const percent = max === 0 ? 0 : (clampedValue / max) * 100;

  // Colors based on state.
  const fillBg = isCompleted ? "bg-surface-success" : "bg-surface-primary-cta";

  return (
    <div className={twMerge(clsx("flex flex-col gap-1.5 w-full", className))}>
      {/* Header row */}
      <div className="flex items-center justify-between gap-2">
        <div className="flex items-center gap-1.5">
          {isCompleted && showCompletedIcon && (
            <CheckIcon className="size-[14px] text-black" aria-hidden="true" />
          )}
          <span className="font-mono font-bold uppercase text-xs tracking-wider text-black">
            {leftLabel}
          </span>
        </div>
        {metricLabel && (
          <span className="font-mono font-bold uppercase text-xs tracking-wider text-black">
            {metricLabel}
          </span>
        )}
      </div>

      {/* Progress track */}
      <div
        role="progressbar"
        aria-valuenow={clampedValue}
        aria-valuemin={0}
        aria-valuemax={max}
        className={clsx(
          "w-full h-4 rounded-none border border-black bg-white p-[2px]",
          "overflow-hidden"
        )}
      >
        <div
          className={twMerge(
            clsx(
              "h-full border border-black",
              fillBg,
              "transition-all duration-300 ease-snap"
            )
          )}
          style={{ width: `${percent}%` }}
        />
      </div>
    </div>
  );
}

export default ProgressBar;
