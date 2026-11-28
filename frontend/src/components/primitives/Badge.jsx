import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import PlayIcon from "@/assets/icons/play.svg?react";

function Badge({
    label = "BADGE TITLE",
    children,
    variant = "default",
    type = "Default",
    disabled = false,
    showIcon = true,
    icon,
    className
}) {
    const isDisabled = disabled;
    const content = children ?? label;

    const typeAliases = {
        default: "Default",
        pink: "Highlight",
        green: "Success",
        red: "Danger",
        black: "Inverse",
        disabled: "Disabled"
    };

    const typeStyles = {
        Default:
            "bg-surface-panel border-stroke-default shadow-elevation-100 text-content-primary",
        Highlight:
            "bg-surface-highlight border-stroke-default shadow-elevation-100 text-content-inverse",
        Success:
            "bg-surface-success border-stroke-default shadow-elevation-100 text-content-primary",
        Danger: "bg-surface-danger border-stroke-default shadow-elevation-100 text-content-inverse",
        Inverse:
            "bg-surface-inverse border-stroke-default shadow-elevation-100 text-content-inverse",
        Disabled:
            "bg-surface-disabled border-stroke-disabled shadow-elevation-0 text-content-muted"
    };

    const requestedType = variant !== "default" ? variant : type;
    const badgeType = isDisabled
        ? "Disabled"
        : (typeAliases[requestedType] ?? requestedType);
    const typeClass = typeStyles[badgeType] ?? typeStyles.Default;

    const baseClasses = clsx(
        "inline-flex w-fit items-center gap-2xs rounded-none border-[length:var(--border-width-subtle)] px-xs py-2xs",
        typeClass,
        className
    );

    return (
        <span className={twMerge(baseClasses)}>
            {showIcon && (
                <span className="flex size-icon-xs shrink-0 items-center justify-center text-current">
                    {icon ?? (
                        <PlayIcon
                            className="size-icon-xs text-current"
                            aria-hidden="true"
                        />
                    )}
                </span>
            )}
            <span className="text-mono-badge whitespace-nowrap">{content}</span>
        </span>
    );
}

export default Badge;
