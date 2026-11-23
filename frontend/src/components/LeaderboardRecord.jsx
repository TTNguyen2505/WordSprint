import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Badge from "./Badge";
import Cell from "./Cell";

const legacyTypes = {
    header: "Header",
    champion: "First Record",
    you: "Active",
    default: "Default"
};

function LeaderboardRecord({
    type,
    state = "Default",
    variant,
    rank = "01",
    playerName = "player_name",
    score = "120,000",
    playTime = "10:00",
    badgeLabel,
    onClick,
    className
}) {
    const recordType = type ?? legacyTypes[variant] ?? "Default";
    const isHeader = recordType === "Header";
    const isFirstRecord = recordType === "First Record";
    const isActive = recordType === "Active";
    const isDefault = recordType === "Default";
    const isHover = state === "Hover";
    const formattedRank = String(rank).padStart(2, "0");
    const formattedScore =
        typeof score === "number" ? score.toLocaleString() : String(score);
    const badgeText =
        badgeLabel ?? (isFirstRecord ? "CHAMPION" : isActive ? "YOU" : "");
    const badge =
        isFirstRecord || isActive ? (
            <Badge
                type={isFirstRecord ? "Highlight" : "Inverse"}
                showIcon={false}
                className="shadow-elevation-100"
            >
                {badgeText}
            </Badge>
        ) : null;

    const rowBackground = isActive
        ? "bg-surface-primary-cta"
        : isDefault && isHover
          ? "bg-surface-primary-subtle"
          : "bg-surface-panel";

    const playerCell = isHeader ? (
        <Cell
            type="Header"
            data="PLAYER"
            className="w-[280px] shrink-0 justify-center"
        />
    ) : isFirstRecord || isActive ? (
        <Cell
            type="Text"
            className="w-[280px] shrink-0 gap-xs"
            onClick={onClick}
        >
            <span className="truncate font-mono text-mono-data">
                {playerName}
            </span>
            {badge}
        </Cell>
    ) : (
        <Cell
            type="Text"
            data={playerName}
            showBadge={false}
            className="w-[280px] shrink-0"
            onClick={onClick}
        />
    );

    const scoreCell = (
        <Cell
            type={isHeader ? "Header" : "Numerical"}
            data={isHeader ? "SCORE" : formattedScore}
            className={clsx("w-[120px] shrink-0", !isHeader && "justify-end")}
        />
    );
    const playTimeCell = (
        <Cell
            type={isHeader ? "Header" : "Numerical"}
            data={isHeader ? "PLAY TIME" : playTime}
            className={clsx("w-[120px] shrink-0", !isHeader && "justify-end")}
        />
    );

    const handleClick = (event) => {
        if (!isHeader) onClick?.(event);
    };

    return (
        <div
            role={isHeader ? undefined : "row"}
            onClick={handleClick}
            className={twMerge(
                clsx(
                    "flex h-12 w-[600px] min-w-[600px] items-start border-[length:var(--border-width-subtle)] border-stroke-default",
                    rowBackground,
                    isDefault &&
                        "transition-colors duration-[var(--duration-fast)] ease-snap hover:bg-surface-primary-subtle",
                    className
                )
            )}
            data-rank={isHeader ? undefined : formattedRank}
            data-type={recordType}
            data-state={state}
        >
            <Cell
                type={isHeader ? "Header" : "Rank"}
                data={isHeader ? "RANK" : formattedRank}
                className="w-[80px] shrink-0 justify-center"
            />
            {playerCell}
            {scoreCell}
            {playTimeCell}
        </div>
    );
}

export default LeaderboardRecord;
