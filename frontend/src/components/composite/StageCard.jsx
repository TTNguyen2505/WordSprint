import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import sampleThumbnail from "../../assets/sample-stage-card-thumbnail.jpg";
import BoltIcon from "../../assets/icons/bolt.svg?react";
import CheckIcon from "../../assets/icons/check.svg?react";
import LockIcon from "../../assets/icons/lock.svg?react";
import PlayIcon from "../../assets/icons/play.svg?react";
import ReplayIcon from "../../assets/icons/replay.svg?react";
import ScoreIcon from "../../assets/icons/score.svg?react";
import Badge from "../primitives/Badge";
import Button from "../primitives/Button";

function StageCard({
    stageId = "STAGE ID",
    status = "available",
    state = "default",
    topic = "Topic",
    level = "A1",
    imageSrc = sampleThumbnail,
    imageAlt = "Stage preview",
    score = 0,
    onAction,
    className
}) {
    const isLocked = status === "locked";
    const isSolved = status === "solved";
    const isHover = state === "hover";
    const isInteractive = !isLocked && typeof onAction === "function";

    const cardBackground = isLocked
        ? "bg-surface-disabled"
        : isSolved
          ? "bg-surface-panel"
          : "bg-surface-primary-cta";
    const cardBorder = isLocked
        ? "border-stroke-disabled"
        : "border-stroke-default";
    const cardShadow = isLocked
        ? "shadow-elevation-0"
        : isHover
          ? "shadow-elevation-400"
          : "shadow-elevation-200";
    const statusType = isLocked ? "Lock" : isSolved ? "Success" : "Highlight";
    const statusLabel = isLocked ? "LOCKED" : isSolved ? "SOLVED" : "AVAILABLE";
    const statusIcon = isLocked ? (
        <LockIcon className="size-full text-current" aria-hidden="true" />
    ) : isSolved ? (
        <CheckIcon className="size-full text-current" aria-hidden="true" />
    ) : (
        <BoltIcon className="size-full text-current" aria-hidden="true" />
    );
    const actionLabel = isLocked ? "Locked" : isSolved ? "Replay" : "Play";
    const actionVariant = isLocked
        ? "disabled"
        : isSolved
          ? "secondary"
          : "inverse";
    const actionIcon = isLocked ? (
        <LockIcon className="size-icon-xs text-current" aria-hidden="true" />
    ) : isSolved ? (
        <ReplayIcon className="size-icon-xs text-current" aria-hidden="true" />
    ) : (
        <PlayIcon className="size-icon-xs text-current" aria-hidden="true" />
    );

    const handleAction = () => {
        if (isInteractive) onAction();
    };

    return (
        <div
            className={twMerge(
                clsx(
                    "flex w-fit max-w-full flex-col items-start gap-md rounded-none border-[length:var(--border-width-heavy)] px-lg py-md",
                    cardBackground,
                    cardBorder,
                    cardShadow,
                    !isLocked &&
                        "transition-[box-shadow,translate] duration-[var(--duration-normal)] ease-snap hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] hover:shadow-elevation-400",
                    isHover &&
                        !isLocked &&
                        "-translate-x-[var(--primitive-depth-50)] -translate-y-[var(--primitive-depth-50)]",
                    className
                )
            )}
        >
            <div className="flex w-full items-center justify-between">
                <Badge type="Default" showIcon={false} disabled={isLocked}>
                    {stageId}
                </Badge>
                <Badge
                    type={statusType}
                    showIcon
                    icon={statusIcon}
                    disabled={isLocked}
                >
                    {statusLabel}
                </Badge>
            </div>

            <div
                className={twMerge(
                    clsx(
                        "relative aspect-[21/9] w-[270px] shrink-0 overflow-hidden rounded-none border-[length:var(--border-width-default)]",
                        isLocked
                            ? "border-stroke-disabled"
                            : "border-stroke-default",
                        isLocked && "opacity-50"
                    )
                )}
            >
                {imageSrc && (
                    <img
                        src={imageSrc}
                        alt={imageAlt}
                        className="absolute inset-0 size-full object-cover"
                    />
                )}
                <div className="absolute inset-0 flex items-center justify-center">
                    <div
                        className={twMerge(
                            clsx(
                                "flex flex-col items-center justify-center rounded-none border-[length:var(--border-width-subtle)] px-md py-3xs",
                                isLocked
                                    ? "border-stroke-disabled bg-surface-disabled text-content-muted shadow-elevation-0"
                                    : "border-stroke-default bg-surface-panel text-content-primary shadow-elevation-100"
                            )
                        )}
                    >
                        <span className="text-heading-md">{topic}</span>
                        <span className="text-body-sm">{level}</span>
                    </div>
                </div>
            </div>

            <div
                className={twMerge(
                    "h-0 w-full border-t-[length:var(--border-width-default)]",
                    isLocked
                        ? "border-stroke-disabled"
                        : "border-stroke-default"
                )}
            />

            <div className="flex w-full items-center justify-between">
                <Badge
                    type="Default"
                    disabled={isLocked}
                    icon={
                        <ScoreIcon
                            className="size-full text-current"
                            aria-hidden="true"
                        />
                    }
                >
                    {score}
                </Badge>
                <Button
                    label={actionLabel}
                    variant={actionVariant}
                    size="small"
                    leadingIcon={actionIcon}
                    showTrailingIcon={false}
                    onClick={handleAction}
                    disabled={isLocked}
                />
            </div>
        </div>
    );
}

export default StageCard;
