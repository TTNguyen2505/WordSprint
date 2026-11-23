import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Badge from "./Badge";
import DeleteIcon from "../assets/icons/delete.svg?react";

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
    className
}) {
    const isHeader = variant === "header";
    const isYou = variant === "you";

    // Base row classes
    const baseRowClasses = clsx(
        "w-full grid grid-cols-[1fr_104px_64px] items-center px-md min-h-[48px] rounded-none transition-colors duration-150 cursor-pointer select-none",
        {
            // Header variant – static styling
            "bg-surface-panel font-sans font-bold uppercase text-xs tracking-wider text-content-primary cursor-default":
                isHeader,
            // You (active) variant – vibrant yellow background
            "bg-surface-primary-cta font-mono text-sm text-content-primary font-bold":
                isYou,
            // Default variant – white background, subtle yellow hover effect
            "bg-surface-panel font-mono text-sm text-content-primary hover:bg-surface-primary-subtle":
                !isHeader && !isYou
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
        <Badge variant="black" showIcon={false} className="ml-xs px-xs py-3xs">
            {badgeLabel}
        </Badge>
    ) : null;

    return (
        <div
            role={isHeader ? undefined : "row"}
            onClick={handleRowClick}
            data-player={isHeader ? undefined : playerName}
            data-variant={variant}
            className={twMerge(baseRowClasses)}
        >
            {/* PLAYER column */}
            <div className="flex items-center gap-xs min-w-0 overflow-hidden">
                <span>{isHeader ? "PLAYER" : playerName}</span>
                {youBadge}
            </div>

            {/* PROGRESS column */}
            <div className="text-right tabular-nums font-mono font-medium">
                {isHeader ? "PROGRESS" : progress}
            </div>

            {/* ACTION column – delete button only for default variant */}
            <div className="flex items-center justify-end">
                {!isHeader && !isYou && (
                    <button
                        type="button"
                        aria-label={`${deleteAriaLabel} ${playerName}`}
                        data-delete-player={playerName}
                        onClick={handleDelete}
                        className={twMerge(
                            clsx(
                                "w-7 h-7 bg-surface-panel border-2 border-stroke-default rounded-none shadow-[2px_2px_0px_#000]",
                                "inline-flex items-center justify-center text-content-primary",
                                "cursor-pointer hover:bg-surface-primary-cta active:translate-x-[1px] active:translate-y-[1px] active:shadow-none transition-all"
                            )
                        )}
                    >
                        <DeleteIcon
                            className="size-icon-xs text-content-primary"
                            aria-hidden="true"
                        />
                    </button>
                )}
            </div>
        </div>
    );
}

export default ProfileRecord;
