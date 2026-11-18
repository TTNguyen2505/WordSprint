import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";

/**
 * Neo‑Brutalism TextButton component.
 * Inline text‑action button with bold underlined label and yellow highlight on hover/active.
 *
 * Props:
 *   - label (string) – fallback label text (default: "Label").
 *   - children (ReactNode) – content to display, takes precedence over `label`.
 *   - onClick (function) – click handler.
 *   - href (string) – if provided renders an <a> element instead of a <button>.
 *   - type (string) – button type, default "button".
 *   - disabled (boolean) – disables interaction, default false.
 *   - active (boolean) – forces highlighted yellow state, default false.
 *   - className (string) – optional extra Tailwind classes.
 */
function TextButton({
  label = "Label",
  children,
  onClick,
  href,
  type = "button",
  disabled = false,
  active = false,
  className,
}) {
  const isDisabled = disabled;
  const isActive = active;
  const content = children ?? label;

  // Base styles
  const baseClasses = clsx(
    "inline-flex items-center justify-center w-fit",
    "rounded-none",
    "font-bold",
    "underline decoration-2 underline-offset-4",
    "bg-transparent",
    "text-content-primary",
    "hover:underline",
    !isDisabled && "cursor-pointer",
    isDisabled && "cursor-not-allowed opacity-50 text-content-muted underline decoration-2 underline-offset-4",
    !isDisabled && (isActive ? "bg-surface-primary-cta" : "hover:bg-surface-primary-cta focus-visible:bg-surface-primary-cta"),
    className
  );

  // Render as <a> when href is given, otherwise <button>
  if (href) {
    return (
      <a
        href={href}
        onClick={isDisabled ? undefined : onClick}
        className={twMerge(baseClasses)}
        aria-disabled={isDisabled}
      >
        {content}
      </a>
    );
  }

  return (
    <button
      type={type}
      onClick={isDisabled ? undefined : onClick}
      disabled={isDisabled}
      className={twMerge(baseClasses)}
      aria-disabled={isDisabled}
    >
      {content}
    </button>
  );
}

export default TextButton;
