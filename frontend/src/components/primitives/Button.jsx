import clsx from "clsx";
import PlayIcon from "../../assets/icons/play.svg?url";
import { twMerge } from "tailwind-merge";

const sizeStyles = {
    small: {
        container: "gap-3xs px-xs py-2xs",
        icon: "size-icon-xs",
        label: "text-button-sm",
        states: {
            default: "shadow-elevation-100",
            hover: "shadow-elevation-200 -translate-x-[var(--primitive-depth-50)] -translate-y-[var(--primitive-depth-50)]",
            pressed:
                "shadow-elevation-50 translate-x-[var(--primitive-depth-100)] translate-y-[var(--primitive-depth-100)]"
        },
        interactions:
            "hover:shadow-elevation-200 hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] active:shadow-elevation-50 active:translate-x-[var(--primitive-depth-100)] active:translate-y-[var(--primitive-depth-100)]"
    },
    default: {
        container: "gap-xs min-w-[120px] px-md py-xs",
        icon: "size-icon-sm",
        label: "text-button-default",
        states: {
            default: "shadow-elevation-200",
            hover: "shadow-elevation-300 -translate-x-[var(--primitive-depth-50)] -translate-y-[var(--primitive-depth-50)]",
            pressed:
                "shadow-elevation-50 translate-x-[var(--primitive-depth-100)] translate-y-[var(--primitive-depth-100)]"
        },
        interactions:
            "hover:shadow-elevation-300 hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] active:shadow-elevation-50 active:translate-x-[var(--primitive-depth-100)] active:translate-y-[var(--primitive-depth-100)]"
    },
    large: {
        container: "gap-xs min-w-[120px] px-lg py-sm",
        icon: "size-icon-md",
        label: "text-button-lg",
        states: {
            default: "shadow-elevation-200",
            hover: "shadow-elevation-300 -translate-x-[var(--primitive-depth-50)] -translate-y-[var(--primitive-depth-50)]",
            pressed:
                "shadow-elevation-50 translate-x-[var(--primitive-depth-100)] translate-y-[var(--primitive-depth-100)]"
        },
        interactions:
            "hover:shadow-elevation-300 hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] active:shadow-elevation-50 active:translate-x-[var(--primitive-depth-100)] active:translate-y-[var(--primitive-depth-100)]"
    }
};

const variantStyles = {
    primary:
        "border-stroke-default bg-surface-primary-cta text-content-primary",
    secondary: "border-stroke-default bg-surface-panel text-content-primary",
    destructive: "border-stroke-default bg-surface-danger text-content-inverse",
    inverse: "border-stroke-default bg-surface-inverse text-content-inverse",
    disabled:
        "cursor-not-allowed border-stroke-disabled bg-surface-disabled text-content-muted"
};

function ButtonIcon({ icon, size }) {
    return (
        <span
            aria-hidden="true"
            className={clsx("grid shrink-0 place-items-center", size)}
        >
            {icon ?? (
                <span
                    className="size-full bg-current"
                    style={{
                        maskImage: `url("${PlayIcon}")`,
                        maskPosition: "center",
                        maskRepeat: "no-repeat",
                        maskSize: "contain"
                    }}
                />
            )}
        </span>
    );
}

function Button({
    className,
    label = "Label",
    leadingIcon,
    showLeadingIcon = true,
    showTrailingIcon = true,
    size = "small",
    state = "default",
    trailingIcon,
    variant = "primary",
    disabled = false,
    type = "button",
    ...buttonProps
}) {
    const styles = sizeStyles[size];
    const isDisabled = disabled || variant === "disabled";
    const stateStyle = isDisabled
        ? ""
        : styles.states[state] || styles.states.default;
    const variantStyle = isDisabled
        ? variantStyles.disabled
        : variantStyles[variant] || variantStyles.primary;

    return (
        <button
            {...buttonProps}
            className={clsx(
                twMerge(
                    clsx(
                        "inline-flex items-center justify-center border-[length:var(--border-width-default)] transition-[translate,box-shadow] duration-[var(--duration-normal)] ease-snap",
                        styles.container,
                        variantStyle,
                        stateStyle,
                        !isDisabled &&
                            state === "default" &&
                            styles.interactions,
                        isDisabled ? "cursor-not-allowed" : "cursor-pointer"
                    ),
                    className
                ),
                styles.label
            )}
            data-size={size}
            data-state={state}
            data-variant={variant}
            disabled={isDisabled}
            type={type}
        >
            {showLeadingIcon && (
                <ButtonIcon icon={leadingIcon} size={styles.icon} />
            )}
            <span>{label}</span>
            {showTrailingIcon && (
                <ButtonIcon icon={trailingIcon} size={styles.icon} />
            )}
        </button>
    );
}

export default Button;
