import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import PlayIcon from "../assets/icons/play.svg?react";

/**
 * Neo‑Brutalism InfoPill component.
 * Displays a key/value pair with optional leading and trailing icons.
 *
 * Props:
 *   - label (string) – text for the key (default: "Key").
 *   - value (string|number) – value text (default: "Value").
 *   - variant ("default" | "pink" | "green" | "red" | "black" | "disabled") – visual variant (default: "default").
 *   - disabled (boolean) – forces disabled styling (default: false).
 *   - showLeadingIcon (boolean) – whether to render a leading icon (default: true).
 *   - leadingIcon (ReactNode) – custom leading icon; falls back to a play triangle.
 *   - showTrailingIcon (boolean) – whether to render a trailing icon (default: true).
 *   - trailingIcon (ReactNode) – custom trailing icon; falls back to a play triangle.
 *   - className (string) – optional extra Tailwind classes.
 */
function InfoPill({
  label = "Key",
  value = "Value",
  variant = "default",
  disabled = false,
  showLeadingIcon = true,
  leadingIcon,
  showTrailingIcon = true,
  trailingIcon,
  className,
}) {
  const isDisabled = disabled;
  const appliedVariant = isDisabled ? "disabled" : variant;

  const variantStyles = {
    default: "bg-white border-black shadow-elevation-200 text-black",
    pink: "bg-surface-highlight border-black shadow-elevation-200 text-white",
    green: "bg-surface-success border-black shadow-elevation-200 text-black",
    red: "bg-surface-danger border-black shadow-elevation-200 text-white",
    black: "bg-black border-black shadow-elevation-200 text-white",
    disabled: "bg-surface-disabled border-gray-400 shadow-none text-content-muted",
  };

  const variantClass = variantStyles[appliedVariant] ?? variantStyles.default;

  const baseClasses = clsx(
    "inline-flex items-center gap-2 rounded-none px-3 py-1.5 w-fit",
    !isDisabled && "cursor-pointer",
    isDisabled && "cursor-not-allowed opacity-50",
    variantClass,
    className
  );

  return (
    <span className={twMerge(baseClasses)}>
      {showLeadingIcon && (
        <span className="flex items-center">{leadingIcon ?? <PlayIcon className="size-[12px]" aria-hidden="true" />}</span>
      )}
      <span className="inline-flex items-center gap-1.5 whitespace-nowrap">
        <span className="text-xs font-medium text-current">{label}</span>
        <span className="text-xs font-bold text-current">{value}</span>
      </span>
      {showTrailingIcon && (
        <span className="flex items-center">{trailingIcon ?? <PlayIcon className="size-[12px]" aria-hidden="true" />}</span>
      )}
    </span>
  );
}

export default InfoPill;
