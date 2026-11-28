import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Toggle from "./Toggle";

function Slider({
    title = "Title",
    enabled = true,
    onToggle,
    showToggle = true,
    value = 50,
    min = 0,
    max = 100,
    step = 1,
    onChange,
    className,
    id
}) {
    const isDisabled = !enabled;
    const clampedValue = Math.min(Math.max(value, min), max);
    const percent =
        max === min ? 0 : ((clampedValue - min) / (max - min)) * 100;

    const cardBorder = isDisabled
        ? "border-stroke-disabled"
        : "border-stroke-default";
    const cardShadow = isDisabled
        ? "shadow-elevation-0"
        : "shadow-elevation-200";
    const titleColor = isDisabled
        ? "text-content-muted"
        : "text-content-primary";
    const trackBorder = isDisabled
        ? "border-stroke-disabled"
        : "border-stroke-default";
    const fillColor = isDisabled ? "bg-surface-disabled" : "bg-surface-inverse";
    const fillBorder = isDisabled
        ? "border-stroke-disabled"
        : "border-stroke-default";
    const thumbBorder = isDisabled
        ? "border-stroke-disabled"
        : "border-stroke-default";
    const gripColor = isDisabled ? "bg-stroke-disabled" : "bg-surface-inverse";

    const handleToggle = (nextState) => {
        onToggle?.(nextState);
    };

    const handleRangeChange = (event) => {
        onChange?.(Number(event.target.value));
    };

    return (
        <div
            id={id}
            data-testid={id}
            className={twMerge(
                clsx(
                    "flex w-full max-w-[480px] flex-col gap-lg rounded-none border-[length:var(--border-width-default)] bg-surface-panel px-md py-sm",
                    cardBorder,
                    cardShadow,
                    className
                )
            )}
        >
            <div className="flex items-center justify-between">
                <h2
                    className={twMerge(
                        "font-sans text-[length:var(--primitive-font-size-24)] font-bold leading-[var(--primitive-line-height-30)] tracking-[var(--primitive-letter-spacing-tight)]",
                        titleColor
                    )}
                >
                    {title}
                </h2>
                {showToggle && (
                    <Toggle
                        enabled={enabled}
                        onChange={handleToggle}
                        ariaLabel={`${title} toggle`}
                    />
                )}
            </div>

            <div className="relative flex h-[var(--spacing-xl)] w-full items-center">
                <input
                    type="range"
                    min={min}
                    max={max}
                    step={step}
                    value={clampedValue}
                    onChange={handleRangeChange}
                    disabled={isDisabled}
                    aria-label={title}
                    className={twMerge(
                        "absolute inset-0 z-20 h-full w-full cursor-pointer appearance-none opacity-0",
                        isDisabled && "cursor-not-allowed"
                    )}
                />
                <div
                    className={twMerge(
                        clsx(
                            "relative h-[var(--spacing-xl)] w-full overflow-hidden rounded-none border-[length:var(--border-width-subtle)] bg-surface-panel p-2xs",
                            trackBorder
                        )
                    )}
                >
                    <div
                        className={twMerge(
                            clsx(
                                "h-full border-[length:var(--border-width-subtle)] transition-[width] duration-[var(--duration-fast)] ease-snap",
                                fillColor,
                                fillBorder
                            )
                        )}
                        style={{ width: `${percent}%` }}
                    />
                    <div
                        className={twMerge(
                            clsx(
                                "pointer-events-none absolute top-1/2 z-10 flex size-icon-md items-center justify-center border-[length:var(--border-width-subtle)] bg-surface-panel transition-[left,transform] duration-[var(--duration-fast)] ease-snap",
                                thumbBorder
                            )
                        )}
                        style={{
                            left: `${percent}%`,
                            transform: `translate(-${percent}%, -50%)`
                        }}
                    >
                        <div
                            className="flex h-sm items-center justify-center gap-3xs"
                            aria-hidden="true"
                        >
                            <span
                                className={twMerge(
                                    "block h-sm w-3xs",
                                    gripColor
                                )}
                            />
                            <span
                                className={twMerge(
                                    "block h-sm w-3xs",
                                    gripColor
                                )}
                            />
                            <span
                                className={twMerge(
                                    "block h-sm w-3xs",
                                    gripColor
                                )}
                            />
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
}

export default Slider;
