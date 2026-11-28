import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import PlayIcon from "@/assets/icons/play.svg?react";

function InfoPill({
    label = "Key",
    value = "Value",
    children,
    type = "Metadata",
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
    const typeAliases = {
        default: "Metadata",
        pink: "Highlight",
        green: "Success",
        red: "Danger",
        black: "Inverse",
        disabled: "Lock"
    };
    const requestedType = variant !== "default" ? variant : type;
    const pillType = disabled
        ? "Lock"
        : (typeAliases[requestedType] ?? requestedType);
    const typeStyles = {
        Metadata:
            "bg-surface-panel border-stroke-default shadow-elevation-200 text-content-primary",
        Highlight:
            "bg-surface-highlight border-stroke-default shadow-elevation-200 text-content-inverse",
        Success:
            "bg-surface-success border-stroke-default shadow-elevation-200 text-content-primary",
        Danger: "bg-surface-danger border-stroke-default shadow-elevation-200 text-content-inverse",
        Lock: "bg-surface-disabled border-stroke-disabled shadow-elevation-0 text-content-muted",
        Inverse:
            "bg-surface-inverse border-stroke-default shadow-elevation-200 text-content-inverse"
    };
    const hasLeading = showIcon !== false && showLeadingIcon;
    const typeClass = typeStyles[pillType] ?? typeStyles.Metadata;

    return (
        <div
            id={id}
            data-testid={id}
            className={twMerge(
                clsx(
                    "inline-flex w-fit items-center gap-md rounded-none border-[length:var(--border-width-subtle)] px-sm py-xs",
                    typeClass,
                    className
                )
            )}
        >
            <div className="flex shrink-0 items-center gap-sm">
                {hasLeading && (
                    <span className="flex size-icon-md shrink-0 items-center justify-center text-current">
                        {leadingIcon ?? (
                            <PlayIcon
                                className="size-full text-current"
                                aria-hidden="true"
                            />
                        )}
                    </span>
                )}
                <span className="flex items-center gap-xs whitespace-nowrap text-body-default text-current">
                    <span>{label}</span>
                    <span className="text-body-default-bold">
                        {displayValue}
                    </span>
                </span>
            </div>
            {showTrailingIcon && (
                <span className="flex size-icon-md shrink-0 items-center justify-center text-current">
                    {trailingIcon ?? (
                        <PlayIcon
                            className="size-full text-current"
                            aria-hidden="true"
                        />
                    )}
                </span>
            )}
        </div>
    );
}

export default InfoPill;
