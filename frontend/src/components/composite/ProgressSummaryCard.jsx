import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import BoltIcon from "../../assets/icons/bolt.svg?react";
import CheckIcon from "../../assets/icons/check.svg?react";
import Badge from "../primitives/Badge";
import InfoPill from "../primitives/InfoPill";

function ProgressSummaryCard({
    type = "Next stage",
    status,
    badgeLabel,
    eyebrow,
    title,
    metaLabel,
    metaValue,
    onClick,
    className
}) {
    const resolvedType =
        status === "completed"
            ? "Campaign complete"
            : status === "available"
              ? "Next stage"
              : type;
    const isComplete = resolvedType === "Campaign complete";
    const cfg = isComplete
        ? {
              badgeLabel: "COMPLETED",
              eyebrow: "OVERALL PROGRESS",
              title: "All Stages Conquered!",
              metaLabel: "Campaign Score",
              metaValue: "5420",
              badgeType: "Success"
          }
        : {
              badgeLabel: "AVAILABLE",
              eyebrow: "NEXT STAGE",
              title: "Stage 2: Family",
              metaLabel: "Level",
              metaValue: "A2",
              badgeType: "Highlight"
          };

    const finalBadgeLabel = badgeLabel ?? cfg.badgeLabel;
    const finalEyebrow = eyebrow ?? cfg.eyebrow;
    const finalTitle = title ?? cfg.title;
    const finalMetaLabel = metaLabel ?? cfg.metaLabel;
    const finalMetaValue = metaValue ?? cfg.metaValue;
    const interactive = typeof onClick === "function";

    const containerClasses = twMerge(
        clsx(
            "flex w-full max-w-[640px] flex-col items-start gap-xl rounded-none border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel px-lg py-md shadow-elevation-200",
            interactive &&
                "cursor-pointer transition-[translate,box-shadow] duration-[var(--duration-normal)] ease-snap hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] hover:shadow-elevation-300 active:translate-x-[var(--primitive-depth-100)] active:translate-y-[var(--primitive-depth-100)] active:shadow-elevation-50",
            className
        )
    );

    return (
        <div
            className={containerClasses}
            onClick={onClick}
            role={interactive ? "button" : undefined}
        >
            <div className="flex w-full shrink-0 items-center">
                <Badge
                    type={cfg.badgeType}
                    showIcon
                    icon={
                        isComplete ? (
                            <CheckIcon
                                className="size-icon-xs text-current"
                                aria-hidden="true"
                            />
                        ) : (
                            <BoltIcon
                                className="size-icon-xs text-current"
                                aria-hidden="true"
                            />
                        )
                    }
                >
                    {finalBadgeLabel}
                </Badge>
            </div>

            <div className="flex w-full shrink-0 items-end justify-between">
                <div className="flex shrink-0 flex-col gap-sm leading-[var(--primitive-line-height-30)] text-content-primary">
                    <p className="text-subheading-lg">{finalEyebrow}</p>
                    <h2 className="text-heading-lg">{finalTitle}</h2>
                </div>
                <InfoPill
                    type="Metadata"
                    label={finalMetaLabel}
                    value={finalMetaValue}
                    showLeadingIcon={false}
                    showTrailingIcon={false}
                    className="gap-md px-sm py-xs shadow-elevation-200"
                />
            </div>
        </div>
    );
}

export default ProgressSummaryCard;
