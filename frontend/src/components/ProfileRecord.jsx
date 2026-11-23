import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Badge from "./Badge";
import Cell from "./Cell";

const legacyTypes = {
    header: "Header",
    you: "Active",
    default: "Default"
};

function ProfileRecord({
    type,
    variant,
    state = "Default",
    playerName = "nguyen_03",
    progress = "6/12",
    badgeLabel = "YOU",
    onSelect,
    onDelete,
    deleteAriaLabel = "Delete profile",
    className
}) {
    const recordType = type ?? legacyTypes[variant] ?? "Default";
    const isHeader = recordType === "Header";
    const isActive = recordType === "Active";
    const isDefault = recordType === "Default";
    const isHover = state === "Hover";
    const rowBackground = isActive
        ? "bg-surface-primary-cta"
        : isDefault && isHover
          ? "bg-surface-primary-subtle"
          : "bg-surface-panel";

    const handleSelect = (event) => {
        if (!isHeader) onSelect?.(event);
    };

    const handleDelete = (event) => {
        event.stopPropagation();
        onDelete?.(event);
    };

    const playerCell = isHeader ? (
        <Cell
            type="Header"
            data="PLAYER"
            className="w-[280px] shrink-0 justify-center"
        />
    ) : isActive ? (
        <Cell type="Text" className="w-[280px] shrink-0 gap-xs">
            <span className="text-mono-data truncate">{playerName}</span>
            <Badge type="Inverse" showIcon={false}>
                {badgeLabel}
            </Badge>
        </Cell>
    ) : (
        <Cell
            type="Text"
            data={playerName}
            showBadge={false}
            className="w-[280px] shrink-0"
        />
    );

    const progressCell = (
        <Cell
            type={isHeader ? "Header" : "Numerical"}
            data={isHeader ? "PROGRESS" : progress}
            className={clsx("w-[120px] shrink-0", !isHeader && "justify-end")}
        />
    );

    return (
        <div
            role={isHeader ? undefined : "row"}
            onClick={handleSelect}
            className={twMerge(
                clsx(
                    "flex w-[520px] min-w-[520px] items-start border-[length:var(--border-width-subtle)] border-stroke-default",
                    rowBackground,
                    isDefault &&
                        "transition-colors duration-[var(--duration-fast)] ease-snap hover:bg-surface-primary-subtle",
                    !isHeader && onSelect && "cursor-pointer",
                    className
                )
            )}
            data-type={recordType}
            data-state={state}
        >
            {playerCell}
            {progressCell}
            {isHeader ? (
                <Cell
                    type="Header"
                    data=""
                    className="w-[120px] shrink-0 justify-center"
                />
            ) : isActive ? (
                <div className="h-[var(--primitive-space-48)] w-[120px] shrink-0" />
            ) : (
                <Cell
                    type="Action"
                    onAction={handleDelete}
                    actionLabel={deleteAriaLabel}
                    className="w-[120px] shrink-0 justify-center"
                />
            )}
        </div>
    );
}

export default ProfileRecord;
