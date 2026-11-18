import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Toggle from "./Toggle";

/**
 * Neo‑Brutalism Slider card component.
 *
 * Props:
 *   - title (string) – header text, defaults to "Title".
 *   - enabled (boolean) – enables/disables the slider and its Toggle, defaults to true.
 *   - onToggle (function) – called with the new enabled state when the Toggle is clicked.
 *   - value (number) – current numeric value, defaults to 50.
 *   - min (number) – minimum value, defaults to 0.
 *   - max (number) – maximum value, defaults to 100.
 *   - step (number) – step interval, defaults to 1.
 *   - onChange (function) – called with the updated value when the range changes.
 *   - className (string) – optional extra Tailwind classes for the outer card.
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
}) {
  const isDisabled = !enabled;

  // Clamp value between min and max.
  const clampedValue = Math.min(Math.max(value, min), max);
  const percent = ((clampedValue - min) / (max - min)) * 100;

  const handleToggle = () => {
    if (typeof onToggle === "function") {
      onToggle(!enabled);
    }
  };

  const handleRangeChange = (e) => {
    const newVal = Number(e.target.value);
    if (typeof onChange === "function") {
      onChange(newVal);
    }
  };

  // Styling based on enabled/disabled state.
  const cardBorder = isDisabled ? "border-gray-400" : "border-black";
  const cardShadow = isDisabled ? "" : "shadow-elevation-200";
  const titleColor = isDisabled ? "text-content-muted" : "text-content-primary";
  const trackBorder = isDisabled ? "border-gray-400" : "border-black";
  const fillColor = isDisabled ? "bg-surface-disabled" : "bg-black";
  const thumbBorder = isDisabled ? "border-gray-400" : "border-black";
  const gripColor = isDisabled ? "text-content-muted" : "text-content-primary";

  return (
    <div
      className={twMerge(
        clsx(
          "flex flex-col gap-y-4 rounded-none border-2",
          cardBorder,
          cardShadow,
          "bg-white p-4",
          className
        ),
        "max-w-[426px] w-full"
      )}
    >
      {/* Header */}
      <div className="flex items-center justify-between">
        <h2 className={twMerge("font-bold text-base", titleColor)}>{title}</h2>
        <Toggle
          enabled={enabled}
          onChange={handleToggle}
          disabled={isDisabled}
          ariaLabel={title + " toggle"}
        />
      </div>

      {/* Slider track */}
      <div className="relative flex items-center">
        {/* Hidden range input for accessibility */}
        <input
          type="range"
          min={min}
          max={max}
          step={step}
          value={clampedValue}
          onChange={handleRangeChange}
          disabled={isDisabled}
          className="absolute inset-0 w-full h-full opacity-0 appearance-none cursor-pointer"
        />
        {/* Track background */}
        <div
          className={twMerge(
            clsx(
              "w-full h-[28px] rounded-none border-2",
              trackBorder,
              "bg-white p-[2px]"
            )
          )}
        >
          {/* Filled portion */}
          <div
            className={twMerge(
              clsx(
                "h-full",
                fillColor,
                "transition-width duration-150 ease-snap"
              )
            )}
            style={{ width: `${percent}%` }}
          />
        </div>
        {/* Thumb handle */
        <div
          className={twMerge(
            clsx(
              "size-[24px] bg-white border-2",
              thumbBorder,
              "absolute top-1/2 left-0 transform -translate-y-1/2 transition-transform duration-150 ease-snap",
              isDisabled && "opacity-60"
            )
          )}
          style={{ left: `calc(${percent}% - 12px)` }} // 12px = half thumb size
        >
          {/* Grip lines */}
          <div className="flex h-full items-center justify-center gap-[2px]" aria-hidden="true">
            <span className={twMerge("w-[2px] h-[12px]", gripColor)}></span>
            <span className={twMerge("w-[2px] h-[12px]", gripColor)}></span>
            <span className={twMerge("w-[2px] h-[12px]", gripColor)}></span>
          </div>
        </div>
      </div>
    </div>
  );
}

export default Slider;
