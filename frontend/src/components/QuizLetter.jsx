import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";

/**
 * Neo‑Brutalism QuizLetter component – a square tile used for quiz slots.
 *
 * Props:
 *   - letter (string) – character to display; empty string renders an underscore placeholder.
 *   - onClick (function) – optional click handler; when provided the tile is interactive.
 *   - disabled (boolean) – disables interaction and applies muted styling (default false).
 *   - status ("default" | "selected" | "correct" | "error") – visual state variant.
 *       * default – white background with black border and offset shadow.
 *       * selected – same base but could be highlighted later (currently maps to default).
 *       * correct – uses the success (green) accent background.
 *       * error – uses the danger (red) accent background.
 *   - className (string) – optional extra Tailwind classes.
 */
function QuizLetter({
  letter = "",
  onClick,
  disabled = false,
  status = "default",
  className,
}) {
  const isDisabled = disabled;
  const hasLetter = String(letter).trim().length > 0;

  // Base background based on status
  const bgMap = {
    default: "bg-white",
    selected: "bg-white",
    correct: "bg-surface-success",
    error: "bg-surface-danger",
  };

  const bgClass = bgMap[status] ?? bgMap.default;

  const baseClasses = clsx(
    "inline-flex items-center justify-center shrink-0 w-12 h-12",
    "rounded-none border-2 border-black",
    bgClass,
    "shadow-[4px_4px_0px_#000]",
    !isDisabled && onClick && "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] active:translate-x-[2px] active:translate-y-[2px] transition-transform",
    isDisabled && "opacity-50 cursor-not-allowed",
    className
  );

  const handleClick = (e) => {
    if (!isDisabled && typeof onClick === "function") {
      onClick(e);
    }
  };

  return (
    <div
      role="button"
      onClick={handleClick}
      className={twMerge(baseClasses)}
    >
      {hasLetter ? (
        <span className="text-xl font-bold uppercase text-black select-none">
          {String(letter).toUpperCase()}
        </span>
      ) : (
        <span className="relative w-3.5 h-[2.5px] bg-black" />
      )}
    </div>
  );
}

export default QuizLetter;
