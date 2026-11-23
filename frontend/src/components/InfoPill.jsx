import clsx from "clsx";
import { twMerge } from "tailwind-merge";
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
 *   - showIcon (boolean) – compatibility flag for leading icon visibility.
 *   - showLeadingIcon (boolean) – whether to render a leading icon (default: true).
 *   - leadingIcon (ReactNode) – custom leading icon; falls back to a play triangle.
 *   - showTrailingIcon (boolean) – whether to render a trailing icon (default: true).
 *   - trailingIcon (ReactNode) – custom trailing icon; falls back to a play triangle.
 *   - className (string) – optional extra Tailwind classes.
 *   - children (ReactNode) – value fallback if provided.
 */
function InfoPill({
    label = "Key",
    value = "Value",
    children,
    variant = "default",
    disabled = false,
    showIcon,
    showLeadingIcon = true,
    leadingIcon,
    showTrailingIcon = true,
    trailingIcon,
    className,
    id
}) {
    const displayValue = children ?? value;
    const isDisabled = disabled;
    const appliedVariant = isDisabled ? "disabled" : variant;

    const variantStyles = {
        default:
            "bg-surface-panel border-stroke-default shadow-elevation-200 text-content-primary",
        pink: "bg-surface-highlight border-stroke-default shadow-elevation-200 text-content-inverse",
        green: "bg-surface-success border-stroke-default shadow-elevation-200 text-content-primary",
        red: "bg-surface-danger border-stroke-default shadow-elevation-200 text-content-inverse",
        black: "bg-surface-inverse border-stroke-default shadow-elevation-200 text-content-inverse",
        disabled:
            "bg-surface-disabled border-stroke-disabled shadow-elevation-0 text-content-muted"
    };

    const variantClass = variantStyles[appliedVariant] ?? variantStyles.default;
    const hasLeading = showIcon !== false && showLeadingIcon;

    const baseClasses = clsx(
        "inline-flex items-center justify-center gap-xs rounded-none px-md py-xs h-[38px] border-2 box-border transition-all",
        !isDisabled && "cursor-pointer",
        isDisabled && "cursor-not-allowed opacity-50",
        variantClass,
        className
    );

    return (
        <div id={id} data-testid={id} className={twMerge(baseClasses)}>
            {hasLeading && (
                <span className="flex items-center">
                    {leadingIcon ?? (
                        <PlayIcon className="size-[12px]" aria-hidden="true" />
                    )}
                </span>
            )}
            <span className="inline-flex items-center gap-xs whitespace-nowrap">
                <span className="text-body-default text-current">{label}</span>
                <span className="text-body-default-bold text-current">
                    {displayValue}
                </span>
            </span>
            {showTrailingIcon && (
                <span className="flex items-center">
                    {trailingIcon ?? (
                        <PlayIcon className="size-[12px]" aria-hidden="true" />
                    )}
                </span>
            )}
        </div>
    );
}

export default InfoPill;
