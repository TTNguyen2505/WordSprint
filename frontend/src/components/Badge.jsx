import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import PlayIcon from "../assets/icons/play.svg?react";

/**
 * Neo‑Brutalism Badge component.
 * Inline tag/pill with optional leading icon and variant styling.
 *
 * Props:
 *   - label (string) – fallback text, defaults to "BADGE TITLE".
 *   - children (ReactNode) – content to display, takes precedence over `label`.
 *   - variant (string) – one of "default", "pink", "green", "red", "black", "disabled"; default "default".
 *   - disabled (boolean) – forces disabled styling, default false.
 *   - showIcon (boolean) – whether to show a leading icon, default true.
 *   - icon (ReactNode) – custom leading icon; if omitted and `showIcon` is true, renders a default play triangle.
 *   - className (string) – optional extra Tailwind classes.
 */
function Badge({
    label = "BADGE TITLE",
    children,
    variant = "default",
    disabled = false,
    showIcon = true,
    icon,
    className
}) {
    const isDisabled = disabled;
    const content = children ?? label;

    const variantStyles = {
        default:
            "bg-surface-panel border-stroke-default shadow-elevation-200 text-content-primary",
        pink: "bg-surface-highlight border-stroke-default shadow-elevation-200 text-content-inverse",
        green: "bg-surface-success border-stroke-default shadow-elevation-200 text-content-primary",
        red: "bg-surface-danger border-stroke-default shadow-elevation-200 text-content-inverse",
        black: "bg-surface-inverse border-stroke-default shadow-elevation-200 text-content-inverse",
        disabled:
            "bg-surface-disabled border-stroke-disabled shadow-none text-content-muted"
    };

    const appliedVariant = isDisabled ? "disabled" : variant;
    const variantClass = variantStyles[appliedVariant] ?? variantStyles.default;

    const baseClasses = clsx(
        "inline-flex items-center gap-xs rounded-none px-sm py-3xs w-fit border border-stroke-default",
        !isDisabled && "cursor-pointer",
        isDisabled && "cursor-not-allowed opacity-50",
        variantClass,
        className
    );

    return (
        <span className={twMerge(baseClasses)}>
            {showIcon && (
                <span className="flex items-center">
                    {icon ?? (
                        <PlayIcon className="size-[12px]" aria-hidden="true" />
                    )}
                </span>
            )}
            <span className="font-mono font-bold uppercase text-[11px] leading-none tracking-wider whitespace-nowrap">
                {content}
            </span>
        </span>
    );
}

export default Badge;
