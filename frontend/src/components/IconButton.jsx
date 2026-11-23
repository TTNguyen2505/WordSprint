import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import PlayIcon from "../assets/icons/play.svg?react";

const sizeStyles = {
    Small: "size-icon-sm",
    "Extra Small": "size-icon-xs"
};

const stateStyles = {
    Default:
        "border-stroke-default bg-surface-panel shadow-elevation-100 text-content-primary",
    Hover: "border-stroke-default bg-surface-primary-cta shadow-elevation-200 text-content-primary",
    Pressed:
        "border-stroke-default bg-surface-canvas shadow-elevation-0 text-content-primary",
    Disabled:
        "border-stroke-disabled bg-surface-disabled shadow-elevation-0 text-content-muted"
};

function IconButton({
    icon,
    size = "Small",
    state = "Default",
    onClick,
    ariaLabel = "Action",
    className,
    type = "button"
}) {
    const isDisabled = state === "Disabled";
    const iconSize = sizeStyles[size] ?? sizeStyles.Small;
    const appearance = stateStyles[state] ?? stateStyles.Default;

    return (
        <button
            type={type}
            aria-label={ariaLabel}
            disabled={isDisabled}
            onClick={onClick}
            className={twMerge(
                clsx(
                    "inline-flex w-fit items-center justify-center rounded-none border-[length:var(--border-width-subtle)] p-2xs transition-[background-color,border-color,box-shadow] duration-[var(--duration-normal)] ease-snap motion-reduce:transition-none",
                    appearance,
                    !isDisabled && "cursor-pointer",
                    !isDisabled &&
                        state === "Default" &&
                        "hover:bg-surface-primary-cta hover:shadow-elevation-200",
                    !isDisabled &&
                        "active:bg-surface-canvas active:shadow-elevation-0",
                    isDisabled && "cursor-not-allowed",
                    className
                )
            )}
            data-size={size}
            data-state={state}
        >
            <span
                className={clsx("grid shrink-0 place-items-center", iconSize)}
            >
                {icon ?? (
                    <PlayIcon
                        className="size-full text-current"
                        aria-hidden="true"
                    />
                )}
            </span>
        </button>
    );
}

export default IconButton;
