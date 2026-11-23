import clsx from "clsx";
import { twMerge } from "tailwind-merge";
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
    className
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
        typeof score === "number" ? score.toLocaleString() : String(score);

    // Badge handling.
    let badge = null;
    if (isChampion || isYou) {
        const label = badgeLabel ?? (isChampion ? "CHAMPION" : "YOU");
        const badgeVariant = isChampion ? "pink" : "black";
        badge = (
            <Badge
                variant={badgeVariant}
                showIcon={false}
                className={clsx(
                    "ml-2.5 shadow-elevation-100",
                    isChampion ? "badge-champion" : "badge-you"
                )}
            >
                {label}
            </Badge>
        );
    }

    // Base row classes.
    const baseRowClasses = clsx(
        "w-full grid grid-cols-[80px_1fr_120px_120px] items-center px-2 sm:px-4 min-h-[48px] border-b border-stroke-default rounded-none last:border-b-0",
        {
            // Header variant – static styling.
            "bg-surface-panel font-sans font-bold uppercase text-base text-content-primary":
                isHeader,
            // Champion and default variants share base font.
            "bg-surface-panel font-mono text-sm text-content-primary":
                !isHeader,
            // Default hover background.
            "hover:bg-[#FFF5B8]": !isHeader && !isChampion && !isYou,
            // You variant bright yellow background.
            "bg-surface-primary-cta current-player": isYou
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
            data-rank={isHeader ? undefined : formattedRank}
        >
            {/* Rank */}
            <div className="text-center font-mono">
                {isHeader ? "RANK" : formattedRank}
            </div>

            {/* Player column */}
            <div
                className={clsx(
                    "flex items-center gap-2 min-w-0 overflow-hidden",
                    isHeader ? "justify-center" : "justify-start"
                )}
            >
                <span className="truncate">
                    {isHeader ? "PLAYER" : playerName}
                </span>
                {badge}
            </div>

            {/* Score */}
            <div
                className={clsx(
                    "tabular-nums pr-2 sm:pr-4",
                    isHeader ? "text-center font-sans" : "text-right font-mono"
                )}
            >
                {isHeader ? "SCORE" : formattedScore}
            </div>

            {/* Play Time */}
            <div
                className={clsx(
                    "tabular-nums pr-2 sm:pr-4",
                    isHeader ? "text-center font-sans" : "text-right font-mono"
                )}
            >
                {isHeader ? "PLAY TIME" : playTime}
            </div>
        </div>
    );
}

export default LeaderboardRecord;
