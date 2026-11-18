import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import Badge from "./Badge";

/**
 * Neo‑Brutalism Cell component for tables / lists.
 *
 * Props:
 *   - variant ("bold" | "default" | "badge" | "muted" | "action"): visual style.
 *   - value (string|number): content to display (default "01").
 *   - children (ReactNode): overrides default rendering when provided.
 *   - badgeLabel (string): text for the Badge in the "badge" variant (default "BADGE TITLE").
 *   - badgeVariant (string): Badge variant (default "black").
 *   - badgeProps (object): additional props passed to Badge.
 *   - icon (ReactNode): custom icon for the "action" variant.
 *   - onAction (function): click handler for the action button.
 *   - actionLabel (string): aria‑label for the action button (default "Action").
 *   - disabled (boolean): disables the action button.
 *   - as ("div" | "td"): wrapper element (default "div").
 *   - className (string): optional extra Tailwind classes for the outer wrapper.
 */
function Cell({
  variant = "default",
  value = "01",
  children,
  badgeLabel = "BADGE TITLE",
  badgeVariant = "black",
  badgeProps = {},
  icon,
  onAction,
  actionLabel = "Action",
  disabled = false,
  as = "div",
  className,
}) {
  const isAction = variant === "action";
  const isDisabled = disabled;

  // Outer element – div or td
  const Wrapper = as === "td" ? "td" : "div";

  // Content based on variant (or children overrides)
  const renderContent = () => {
    if (children) return children;
    switch (variant) {
      case "bold":
        return <span className="font-bold text-black">{value}</span>;
      case "default":
        return <span className="font-normal text-black">{value}</span>;
      case "badge":
        return (
          <React.Fragment>
            <span className="text-neutral-700">{value}</span>
            <Badge
              variant={badgeVariant}
              showIcon={true}
              className="ml-2.5"
              {...badgeProps}
            >
              {badgeLabel}
            </Badge>
          </React.Fragment>
        );
      case "muted":
        return <span className="text-xs text-neutral-500">{value}</span>;
      case "action":
        return (
          <button
            type="button"
            aria-label={actionLabel}
            disabled={isDisabled}
            onClick={(e) => {
              if (!isDisabled && typeof onAction === "function") onAction(e);
            }}
            className={twMerge(
              clsx(
                "w-7 h-7 bg-white border border-black rounded-none shadow-[2px_2px_0px_#000]",
                "inline-flex items-center justify-center text-black",
                !isDisabled && "cursor-pointer hover:bg-surface-primary-cta active:translate-x-[1px] active:translate-y-[1px] active:shadow-none transition-all",
                isDisabled && "opacity-50 cursor-not-allowed"
              )
            )}
          >
            {icon ?? <span aria-hidden="true">✖</span>}
          </button>
        );
      default:
        return <span>{value}</span>;
    }
  };

  return (
    <Wrapper
      className={twMerge(
        clsx(
          "inline-flex items-center gap-2.5 px-4 py-2.5 min-h-[48px] text-sm font-mono",
          className
        )
      )}
    >
      {renderContent()}
    </Wrapper>
  );
}

export default Cell;
