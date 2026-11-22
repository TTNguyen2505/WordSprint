import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";

/**
 * Neo‑Brutalism Tab component for segmented controls.
 *
 * Props:
 *   - label (string) – fallback text, default "Segmented".
 *   - children (ReactNode) – content to display, takes precedence over `label`.
 *   - active (boolean) – when true, shows the yellow selected style.
 *   - onClick (function) – click handler.
 *   - disabled (boolean) – disables interaction and applies muted styling.
 *   - className (string) – optional extra Tailwind classes.
 */
function Tab({
  label = "Segmented",
  children,
  active = false,
  onClick,
  disabled = false,
  className,
}) {
  const isActive = active && !disabled;
  const isDisabled = disabled;
  const content = children ?? label;

  const baseClasses = clsx(
    "inline-flex items-center justify-center min-w-[120px] h-9 px-4 py-2 rounded-none",
    "border transition-colors duration-150",
    // Default / transparent state
    !isActive && !isDisabled && "bg-transparent border-transparent text-black",
    // Hover state (only when not active/disabled)
    !isActive && !isDisabled && "hover:bg-neutral-100",
    // Active selected state
    isActive && "bg-surface-primary-cta border-black text-black active",
    // Disabled state
    isDisabled && "text-content-muted opacity-50 cursor-not-allowed",
    // Cursor pointer when interactive
    !isDisabled && "cursor-pointer",
    className
  );

  const handleClick = (e) => {
    if (!isDisabled && typeof onClick === "function") {
      onClick(e);
    }
  };

  return (
    <button
      type="button"
      role="tab"
      aria-selected={isActive}
      disabled={isDisabled}
      onClick={handleClick}
      className={twMerge(baseClasses)}
    >
      <span className="font-semibold text-sm text-black whitespace-nowrap">
        {content}
      </span>
    </button>
  );
}

export default Tab;
