import clsx from "clsx";
import { twMerge } from "tailwind-merge";

function Tab({
    label = "Segmented",
    children,
    active = false,
    onClick,
    className
}) {
    const content = children ?? label;

    const baseClasses = clsx(
        "inline-flex w-[120px] items-center justify-center rounded-none px-md py-xs",
        "border-subtle transition-colors duration-[var(--duration-normal)] ease-snap",
        active
            ? "border-stroke-default bg-surface-primary-cta"
            : "border-transparent bg-transparent hover:bg-[var(--primitive-gray-100)]",
        "cursor-pointer motion-reduce:transition-none",
        className
    );

    const handleClick = (e) => {
        if (typeof onClick === "function") {
            onClick(e);
        }
    };

    return (
        <button
            type="button"
            role="tab"
            aria-selected={active}
            onClick={handleClick}
            className={twMerge(baseClasses)}
        >
            <span className="text-button-default whitespace-nowrap text-content-primary">
                {content}
            </span>
        </button>
    );
}

export default Tab;
