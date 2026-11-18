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

  // Background based on state – use existing surface colour tokens.
  const trackBg = isChecked ? "bg-surface-primary-cta" : "bg-surface-disabled";

  // Thumb offset – 0px when off, 28px when on (track width 60px, padding 4px, thumb 24px).
  const thumbTranslate = isChecked ? "translate-x-[28px]" : "translate-x-0";

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
          "inline-flex items-center border-2 border-black",
          "shadow-elevation-200",
          "rounded-none",
          trackBg,
          !isDisabled && "hover:shadow-elevation-300 cursor-pointer",
          isDisabled && "cursor-not-allowed opacity-60",
          className
        ),
        "w-[60px] h-[34px] p-[4px]"
      )}
      data-enabled={isChecked}
    >
      <span
        className={twMerge(
          clsx(
            "size-[24px] bg-white border-2 border-black",
            "transition-transform duration-200 ease-snap",
            thumbTranslate
          )
        )}
      />
    </button>
  );
}

export default Toggle;
