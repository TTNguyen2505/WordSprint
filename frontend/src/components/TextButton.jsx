import clsx from "clsx";
import { twMerge } from "tailwind-merge";

function TextButton({
    label = "Label",
    children,
    onClick,
    href,
    type = "button",
    disabled = false,
    state = "default",
    className
}) {
    const content = children ?? label;

    const baseClasses = twMerge(
        clsx(
            "inline-flex w-fit items-center justify-center rounded-none",
            "bg-transparent text-content-primary underline decoration-from-font [text-underline-position:from-font]",
            "transition-colors duration-[var(--duration-normal)] ease-snap motion-reduce:transition-none",
            disabled
                ? "cursor-not-allowed"
                : "cursor-pointer hover:bg-surface-primary-cta focus-visible:bg-surface-primary-cta",
            !disabled && state === "hover" && "bg-surface-primary-cta"
        ),
        className
    );

    if (href) {
        return (
            <a
                href={disabled ? undefined : href}
                onClick={disabled ? undefined : onClick}
                className={clsx(baseClasses, "text-button-sm")}
                aria-disabled={disabled}
                tabIndex={disabled ? -1 : undefined}
            >
                {content}
            </a>
        );
    }

    return (
        <button
            type={type}
            onClick={disabled ? undefined : onClick}
            disabled={disabled}
            className={clsx(baseClasses, "text-button-sm")}
            aria-disabled={disabled}
        >
            {content}
        </button>
    );
}

export default TextButton;
