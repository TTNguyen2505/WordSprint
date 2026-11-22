import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Toggle from "./Toggle";

/**
 * Neo‑Brutalism Slider card component.
 *
 * Props:
 *   - title (string) – header text, defaults to "Title".
 *   - enabled (boolean) – enables/disables the slider, defaults to true.
 *   - onToggle (function) – called with the new enabled state when the Toggle is clicked.
 *   - value (number) – current numeric value, defaults to 50.
 *   - min (number) – minimum value, defaults to 0.
 *   - max (number) – maximum value, defaults to 100.
 *   - step (number) – step interval, defaults to 1.
 *   - onChange (function) – called with the updated value when the range changes.
 *   - className (string) – optional extra Tailwind classes for the outer card.
 *   - id (string) – optional id / test id.
 */
function Slider({
  title = "Title",
  enabled = true,
  onToggle,
  value = 50,
  min = 0,
  max = 100,
  step = 1,
  onChange,
  className,
  id,
}) {
  const isDisabled = !enabled;

  // Clamp value between min and max.
  const clampedValue = Math.min(Math.max(value, min), max);
  const percent = ((clampedValue - min) / (max - min)) * 100;

  const handleToggle = (nextState) => {
    if (typeof onToggle === "function") {
      onToggle(nextState);
    }
  };

  const handleRangeChange = (e) => {
    const newVal = Number(e.target.value);
    if (typeof onChange === "function") {
      onChange(newVal);
    }
  };

  // Styling based on enabled/disabled state using semantic tokens from index.css
  const cardBorder = isDisabled ? "border-stroke-disabled" : "border-stroke-default";
  const cardShadow = isDisabled ? "shadow-elevation-0" : "shadow-elevation-200";
  const titleColor = isDisabled ? "text-content-muted" : "text-content-primary";
  const trackBorder = isDisabled ? "border-stroke-disabled" : "border-stroke-default";
  const fillColor = isDisabled ? "bg-surface-disabled" : "bg-surface-inverse";
  const thumbBorder = isDisabled ? "border-stroke-disabled" : "border-stroke-default";
  const gripColor = isDisabled ? "bg-stroke-disabled" : "bg-surface-inverse";

  return (
    <div
      id={id}
      data-testid={id}
      className={twMerge(
        clsx(
          "flex flex-col justify-between rounded-none border-2 bg-surface-panel p-4 transition-all duration-150 box-border",
          cardBorder,
          cardShadow,
          "w-[640px] max-w-full h-[126px]",
          className
        )
      )}
    >
      {/* Header */}
      <div className="flex items-center justify-between">
        <h2
          className={twMerge(
            "font-sans font-bold text-[length:var(--primitive-font-size-24)] leading-[var(--primitive-line-height-30)] tracking-tight",
            titleColor
          )}
        >
          {title}
        </h2>
        <Toggle
          enabled={enabled}
          onChange={handleToggle}
          ariaLabel={title + " toggle"}
        />
      </div>

      {/* Slider track */}
      <div className="relative flex items-center h-[32px] w-full">
        {/* Hidden range input for accessibility and drag interaction */}
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
            "absolute inset-0 z-20 h-full w-full opacity-0 appearance-none",
            isDisabled ? "cursor-not-allowed" : "cursor-pointer"
          )}
        />
        {/* Track background */}
        <div
          className={twMerge(
            clsx(
              "w-full h-[32px] rounded-none border-2 bg-surface-panel relative overflow-hidden box-border",
              trackBorder
            )
          )}
        >
          {/* Filled portion */}
          <div
            className={twMerge(
              clsx(
                "h-full transition-[width] duration-100 ease-snap",
                fillColor
              )
            )}
            style={{ width: `${percent}%` }}
          />
        </div>
        {/* Thumb handle */}
        <div
          className={twMerge(
            clsx(
              "size-[24px] bg-surface-panel border-2 absolute top-1/2 pointer-events-none z-10 transition-[left,transform] duration-100 ease-snap box-border flex items-center justify-center",
              thumbBorder
            )
          )}
          style={{
            left: `${percent}%`,
            transform: `translate(-${percent}%, -50%)`,
          }}
        >
          {/* Grip lines */}
          <div className="flex h-[12px] items-center justify-center gap-[2px]" aria-hidden="true">
            <span className={twMerge("w-[2px] h-[12px] block", gripColor)} />
            <span className={twMerge("w-[2px] h-[12px] block", gripColor)} />
            <span className={twMerge("w-[2px] h-[12px] block", gripColor)} />
          </div>
        </div>
      </div>
    </div>
  );
}

export default Slider;
