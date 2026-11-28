import clsx from "clsx";
import { twMerge } from "tailwind-merge";

function Toggle({
    enabled = false,
    onChange,
    className,
    ariaLabel = "Toggle"
}) {
    const isChecked = enabled;

    const trackBg = isChecked
        ? "bg-surface-primary-cta"
        : "bg-surface-disabled";
    const trackBorder = isChecked
        ? "border-stroke-default"
        : "border-stroke-disabled";
    const trackShadow = isChecked
        ? "shadow-elevation-100"
        : "shadow-elevation-0";
    const thumbBorder = isChecked
        ? "border-stroke-default"
        : "border-stroke-disabled";
    const thumbTranslate = isChecked
        ? "translate-x-[calc(100%+var(--primitive-space-2))]"
        : "translate-x-0";

    const handleClick = () => {
        if (typeof onChange === "function") {
            onChange(!enabled);
        }
    };

    return (
        <button
            type="button"
            role="switch"
            aria-checked={isChecked}
            aria-label={ariaLabel}
            onClick={handleClick}
            className={twMerge(
                clsx(
                    "inline-flex w-[60px] items-center rounded-none border-[length:var(--border-width-subtle)] p-2xs transition-[background-color,border-color,box-shadow] duration-[var(--duration-normal)] ease-snap",
                    trackBorder,
                    trackBg,
                    trackShadow,
                    "cursor-pointer",
                    className
                )
            )}
            data-enabled={isChecked}
        >
            <span
                className={twMerge(
                    clsx(
                        "size-icon-md border-[length:var(--border-width-subtle)] bg-surface-panel transition-transform duration-[var(--duration-normal)] ease-snap",
                        thumbBorder,
                        thumbTranslate
                    )
                )}
            />
        </button>
    );
}

export default Toggle;
