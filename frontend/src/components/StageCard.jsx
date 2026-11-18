import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import Badge from "./Badge";

/**
 * Neo‑Brutalism StageCard component.
 *
 * Props:
 *   - stageId (string) – label displayed in the top‑left tag (default: "STAGE ID").
 *   - status ("solved" | "available" | "locked") – visual state (default: "available").
 *   - topic (string) – title shown over the image (default: "Topic").
 *   - level (string) – subtitle (e.g., "A1") displayed under the topic (default: "A1").
 *   - imageSrc (string) – background image URL for the stage preview.
 *   - imageAlt (string) – alt text for the image (default: "Stage preview").
 *   - score (number|string) – points earned for this stage (default: 0).
 *   - onAction (function) – click handler for the action button (Play/Replay).
 *   - className (string) – optional extra Tailwind classes for the outer card.
 */
function StageCard({
  stageId = "STAGE ID",
  status = "available",
  topic = "Topic",
  level = "A1",
  imageSrc,
  imageAlt = "Stage preview",
  score = 0,
  onAction,
  className,
}) {
  const isLocked = status === "locked";
  const isSolved = status === "solved";
  const isAvailable = status === "available";

  // Card background and shadow based on state
  const cardBg = isLocked ? "bg-surface-disabled" : "bg-white";
  const cardBorder = isLocked ? "border-gray-400" : "border-black";
  const cardShadow = isLocked ? "shadow-none" : "shadow-elevation-200";

  // Header badge variant mapping
  const badgeVariant = isSolved ? "green" : isAvailable ? "pink" : "disabled";
  const badgeIcon =
    isSolved ? "✓" : isAvailable ? "⚡" : "🔒";
  const badgeLabel = isSolved
    ? "SOLVED"
    : isAvailable
    ? "AVAILABLE"
    : "LOCKED";

  // Action button styling
  const actionLabel = isSolved ? "Replay" : isAvailable ? "Play" : "Locked";
  const actionBg = isLocked
    ? "bg-surface-disabled"
    : isSolved
    ? "bg-white"
    : "bg-black";
  const actionText = isLocked
    ? "text-content-muted"
    : isSolved
    ? "text-black"
    : "text-white";
  const actionBorder = isLocked ? "border-gray-400" : "border-black";
  const actionShadow = isLocked ? "shadow-none" : "shadow-elevation-200";
  const actionIcon =
    isSolved ? "↺" : isAvailable ? "▶" : "🔒";

  // Points chip styling (same for all states – disabled variant when locked)
  const chipBg = isLocked ? "bg-surface-disabled" : "bg-white";
  const chipBorder = isLocked ? "border-gray-400" : "border-black";
  const chipShadow = isLocked ? "shadow-none" : "shadow-elevation-200";

  // Divider colour
  const dividerBorder = isLocked ? "border-gray-400" : "border-black";

  // Image style – grayscale when locked
  const imageClasses = clsx(
    "w-full h-full object-cover",
    isLocked && "grayscale opacity-50"
  );

  const handleClick = () => {
    if (!isLocked && typeof onAction === "function") {
      onAction();
    }
  };

  return (
    <div
      className={twMerge(
        clsx(
          "flex flex-col gap-3 p-4 rounded-none border-2",
          cardBorder,
          cardBg,
          cardShadow,
          "max-w-[300px] w-full",
          className
        )
      )}
    >
      {/* Header */}
      <div className="flex items-center justify-between gap-2">
        <span className="px-2 py-1 text-[10px] font-mono font-bold uppercase tracking-wider text-black">
          {stageId}
        </span>
        <Badge
          variant={badgeVariant}
          disabled={isLocked}
          className="px-2 py-1"
        >
          {badgeIcon} {badgeLabel}
        </Badge>
      </div>

      {/* Image & overlay */}
      <div className="relative w-full aspect-[16/9] overflow-hidden rounded-none">
        {imageSrc && (
          <img src={imageSrc} alt={imageAlt} className={imageClasses} />
        )}
        {/* Overlay box */}
        <div className="absolute inset-0 flex items-center justify-center p-3">
          <div
            className={twMerge(
              clsx(
                "px-5 py-2.5 text-center rounded-none border-2 border-black bg-white",
                "shadow-elevation-200"
              )
            )}
          >
            <div className="text-base font-bold leading-tight text-black">{topic}</div>
            <div className="mt-0.5 text-xs font-mono text-black">{level}</div>
          </div>
        </div>
      </div>

      {/* Divider */}
      <hr className={twMerge(clsx("border-t", dividerBorder))} />

      {/* Footer */}
      <div className="flex items-center justify-between gap-2">
        {/* Points chip */}
        <span
          className={twMerge(
            clsx(
              "inline-flex items-center gap-1.5 px-2.5 py-1 text-xs font-mono font-bold rounded-none",
              chipBg,
              chipBorder,
              chipShadow,
              "text-black"
            )
          )}
        >
          {/* Simple medal icon using Unicode */}
          <span aria-hidden="true">🏅</span> {score}
        </span>

        {/* Action button */}
        <button
          type="button"
          onClick={handleClick}
          disabled={isLocked}
          className={twMerge(
            clsx(
              "inline-flex items-center gap-1.5 px-3 py-1.5 text-xs font-bold rounded-none",
              actionBg,
              actionText,
              actionBorder,
              actionShadow,
              !isLocked && "cursor-pointer hover:shadow-[6px_6px_0px_#000] hover:-translate-x-[1px] hover:-translate-y-[1px]",
              isLocked && "cursor-not-allowed opacity-50"
            )
          )}
        >
          {actionIcon} {actionLabel}
        </button>
      </div>
    </div>
  );
}

export default StageCard;
