import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import Badge from "./Badge";

/**
 * Neo‑Brutalism ProfileRecord component.
 * Displays a row with player name, progress, and an optional delete action.
 * Supports "header", "you" (active profile) and "default" variants.
 */
function ProfileRecord({
  variant = "default",
  playerName = "nguyen_03",
  progress = "6/12",
  badgeLabel = "YOU",
  onSelect,
  onDelete,
  deleteAriaLabel = "Delete profile",
  className,
}) {
  const isHeader = variant === "header";
  const isYou = variant === "you";

  // Base row classes
  const baseRowClasses = clsx(
    "w-full grid grid-cols-[1fr_104px_64px] items-center px-4 min-h-[48px] border border-black rounded-none transition-colors duration-150",
    {
      // Header variant – static styling
      "bg-white font-sans font-bold uppercase text-xs tracking-wider text-black": isHeader,
      // You (active) variant – yellow background
      "bg-[#FFE600] font-mono text-sm text-black": isYou,
      // Default variant – regular font, hover effect
      "bg-white font-mono text-sm text-black hover:bg-[#FFF5B8]": !isHeader && !isYou,
    },
    className
  );

  // Click handler for the whole row (non‑header rows)
  const handleRowClick = (e) => {
    if (!isHeader && typeof onSelect === "function") {
      onSelect(e);
    }
  };

  // Delete button – stop propagation to avoid triggering row click
  const handleDelete = (e) => {
    e.stopPropagation();
    if (typeof onDelete === "function") {
      onDelete(e);
    }
  };

  // Badge – only for the "you" variant
  const youBadge = isYou ? (
    <Badge variant="black" showIcon={false} className="ml-2.5">
      {badgeLabel}
    </Badge>
  ) : null;

  return (
    <div
      role={isHeader ? undefined : "row"}
      onClick={handleRowClick}
      className={twMerge(baseRowClasses)}
    >
      {/* PLAYER column */}
      <div className="flex items-center gap-2.5 min-w-0 overflow-hidden">
        {isHeader ? "PLAYER" : playerName}
        {youBadge}
      </div>

      {/* PROGRESS column */}
      <div className="text-right tabular-nums">
        {isHeader ? "PROGRESS" : progress}
      </div>

      {/* ACTION column – delete button only for default variant */}
      <div className="flex items-center justify-end">
        {!isHeader && !isYou && (
          <button
            type="button"
            aria-label={deleteAriaLabel}
            onClick={handleDelete}
            className={twMerge(
              clsx(
                "w-7 h-7 bg-white border border-black rounded-none shadow-[2px_2px_0px_#000]",
                "inline-flex items-center justify-center text-black",
                "cursor-pointer hover:bg-[#FFE600] active:translate-x-[1px] active:translate-y-[1px] active:shadow-none transition-all"
              )
            )}
          >
            {/* Simple trash icon using Unicode */}
            <span aria-hidden="true">🗑️</span>
          </button>
        )}
      </div>
    </div>
  );
}

export default ProfileRecord;
