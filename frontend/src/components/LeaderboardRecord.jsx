import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import Badge from "./Badge";

/**
 * Neo‑Brutalism LeaderboardRecord component.
 * Renders a single row for a leaderboard with four visual variants.
 *
 * Props:
 *   - variant ("header" | "champion" | "default" | "you") – visual style.
 *   - rank (number|string) – rank value (default "01").
 *   - playerName (string) – name of the player (default "player_name").
 *   - score (number|string) – player score (default "120,000").
 *   - playTime (string) – formatted play time (default "10:00").
 *   - badgeLabel (string) – custom badge text; defaults to "CHAMPION" for champion variant and "YOU" for you variant.
 *   - onClick (function) – optional click handler for data rows.
 *   - className (string) – optional extra Tailwind classes.
 */
function LeaderboardRecord({
  variant = "default",
  rank = "01",
  playerName = "player_name",
  score = "120,000",
  playTime = "10:00",
  badgeLabel,
  onClick,
  className,
}) {
  const isHeader = variant === "header";
  const isChampion = variant === "champion";
  const isYou = variant === "you";

  // Normalise rank – pad numbers to two digits.
  const formattedRank =
    typeof rank === "number"
      ? String(rank).padStart(2, "0")
      : String(rank).padStart(2, "0");

  // Normalise score – add commas for numbers.
  const formattedScore =
    typeof score === "number"
      ? score.toLocaleString()
      : String(score);

  // Badge handling.
  let badge = null;
  if (isChampion || isYou) {
    const label = badgeLabel ?? (isChampion ? "CHAMPION" : "YOU");
    const badgeVariant = isChampion ? "pink" : "black";
    badge = (
      <Badge
        variant={badgeVariant}
        showIcon={false}
        className="ml-2.5"
      >
        {label}
      </Badge>
    );
  }

  // Base row classes.
  const baseRowClasses = clsx(
    "w-full grid grid-cols-[64px_1fr_112px_96px] items-center px-4 min-h-[48px] border border-black rounded-none",
    {
      // Header variant – static styling.
      "bg-white font-sans font-bold uppercase text-xs tracking-wider text-black": isHeader,
      // Champion and default variants share base font.
      "bg-white font-mono text-sm text-black": !isHeader,
      // Default hover background.
      "hover:bg-[#FFF5B8]": !isHeader && !isChampion && !isYou,
      // You variant bright yellow background.
      "bg-[#FFE600]": isYou,
    },
    className
  );

  // Click handling – only for non‑header rows.
  const handleClick = (e) => {
    if (!isHeader && typeof onClick === "function") {
      onClick(e);
    }
  };

  return (
    <div
      role={isHeader ? undefined : "row"}
      onClick={handleClick}
      className={twMerge(baseRowClasses)}
    >
      {/* Rank */}
      <div className="text-center">
        {isHeader ? "RANK" : formattedRank}
      </div>

      {/* Player column */}
      <div className="flex items-center gap-2.5 min-w-0 overflow-hidden">
        {isHeader ? "PLAYER" : playerName}
        {badge}
      </div>

      {/* Score */}
      <div className="text-right tabular-nums">
        {isHeader ? "SCORE" : formattedScore}
      </div>

      {/* Play Time */}
      <div className="text-right tabular-nums">
        {isHeader ? "PLAY TIME" : playTime}
      </div>
    </div>
  );
}

export default LeaderboardRecord;
