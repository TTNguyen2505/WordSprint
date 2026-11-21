import clsx from "clsx";
import { twMerge } from "tailwind-merge";

/**
 * Neo‑Brutalism Toggle Switch component.
 *
 * Props:
 *   - enabled (boolean, default false) – current state of the toggle.
 *   - onChange (function) – called with the new state when the user clicks.
 *   - disabled (boolean, default false) – disables interaction and applies disabled styling.
 *   - className (string) – optional additional Tailwind classes for the outer track.
 *   - ariaLabel (string, default "Toggle") – accessible label for screen readers.
 */
function Toggle({
  enabled = false,
  onChange,
  disabled = false,
  className,
  ariaLabel = "Toggle",
}) {
  const isDisabled = disabled;
  const isChecked = enabled;

  // Background, borders and shadows based on state using semantic tokens from index.css
  const trackBg = isChecked ? "bg-surface-primary-cta" : "bg-surface-disabled";
  const trackBorder = isChecked ? "border-stroke-default" : "border-stroke-disabled";
  const trackShadow = isChecked ? "shadow-elevation-100" : "shadow-elevation-0";
  const thumbBorder = isChecked ? "border-stroke-default" : "border-stroke-disabled";
  const thumbTranslate = isChecked ? "translate-x-[26px]" : "translate-x-0";

  const handleClick = () => {
    if (!isDisabled && typeof onChange === "function") {
      onChange(!enabled);
    }
  };

  return (
    <button
      type="button"
      role="switch"
      aria-checked={isChecked}
      aria-label={ariaLabel}
      disabled={isDisabled}
      onClick={handleClick}
      className={twMerge(
        clsx(
          "inline-flex items-center border-2 rounded-none transition-all duration-150 box-border",
          trackBorder,
          trackBg,
          trackShadow,
          !isDisabled && "cursor-pointer hover:brightness-105 active:translate-x-[1px] active:translate-y-[1px]",
          isDisabled && "cursor-not-allowed opacity-60",
          className
        ),
        "w-[60px] h-[34px] p-[3px]"
      )}
      data-enabled={isChecked}
    >
      <span
        className={twMerge(
          clsx(
            "size-[24px] bg-surface-panel border-2 transition-transform duration-200 ease-snap block",
            thumbBorder,
            thumbTranslate
          )
        )}
      />
    </button>
  );
}

export default Toggle;
