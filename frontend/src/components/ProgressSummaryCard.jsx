import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import Badge from "./Badge";
import InfoPill from "./InfoPill";

/**
 * Neo‑Brutalism ProgressSummaryCard component.
 * Displays a status badge, an eyebrow, a title, and a key/value metadata pill.
 *
 * Props:
 *   - status ("available" | "completed") – visual variant (default "available").
 *   - badgeLabel (string) – badge text; defaults based on `status`.
 *   - eyebrow (string) – upper‑case subtitle; defaults based on `status`.
 *   - title (string) – main heading; defaults based on `status`.
 *   - metaLabel (string) – label for the InfoPill; defaults based on `status`.
 *   - metaValue (string|number) – value for the InfoPill; defaults based on `status`.
 *   - onClick (function) – optional click handler for the whole card (adds hover/press feedback).
 *   - className (string) – optional extra Tailwind classes.
 */
function ProgressSummaryCard({
  status = "available",
  badgeLabel,
  eyebrow,
  title,
  metaLabel,
  metaValue,
  onClick,
  className,
}) {
  // Determine defaults based on status
  const defaults = {
    available: {
      badgeLabel: "AVAILABLE",
      eyebrow: "NEXT STAGE",
      title: "Stage 2: Family",
      metaLabel: "Level",
      metaValue: "A2",
      badgeVariant: "pink",
    },
    completed: {
      badgeLabel: "COMPLETED",
      eyebrow: "OVERALL PROGRESS",
      title: "All Stages Conquered!",
      metaLabel: "Campaign Score",
      metaValue: "5420",
      badgeVariant: "green",
    },
  };

  const cfg = defaults[status] || defaults.available;

  const finalBadgeLabel = badgeLabel ?? cfg.badgeLabel;
  const finalEyebrow = eyebrow ?? cfg.eyebrow;
  const finalTitle = title ?? cfg.title;
  const finalMetaLabel = metaLabel ?? cfg.metaLabel;
  const finalMetaValue = metaValue ?? cfg.metaValue;
  const badgeVariant = cfg.badgeVariant;

  const interactive = typeof onClick === "function";

  const containerClasses = twMerge(
    clsx(
      "w-full max-w-[660px] min-h-[140px] bg-white border-2 border-black rounded-none",
      "shadow-[4px_4px_0px_#000] p-[20px] flex flex-col justify-between gap-[16px]",
      interactive &&
        "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] hover:shadow-[6px_6px_0px_#000] active:translate-x-[2px] active:translate-y-[2px] active:shadow-none transition-all",
      className
    )
  );

  return (
    <div className={containerClasses} onClick={onClick} role={interactive ? "button" : undefined}>
      {/* Top row – status badge */}
      <div className="flex items-center justify-between">
        <Badge variant={badgeVariant} showIcon={true} className="px-[10px] py-[3px]">
          {finalBadgeLabel}
        </Badge>
      </div>

      {/* Bottom content block */}
      <div className="flex flex-col gap-[6px]">
        <span className="text-xs font-mono font-bold uppercase tracking-wide text-black">
          {finalEyebrow}
        </span>
        <div className="flex flex-wrap items-end justify-between gap-[16px]">
          <h2 className="text-lg font-sans font-bold text-black leading-snug m-0">
            {finalTitle}
          </h2>
          <InfoPill
            label={finalMetaLabel}
            variant="default"
            showIcon={false}
            showTrailingIcon={false}
          >
            {finalMetaValue}
          </InfoPill>
        </div>
      </div>
    </div>
  );
}

export default ProgressSummaryCard;
