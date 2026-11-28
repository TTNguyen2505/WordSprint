import clsx from "clsx";
import { useId, useState } from "react";
import { twMerge } from "tailwind-merge";
import AccountIcon from "../../assets/icons/account.svg?react";
import ErrorIcon from "../../assets/icons/wrong.svg?url";
import SuccessIcon from "../../assets/icons/check.svg?url";

const stateStyles = {
    default: {
        supportingText: "text-content-muted"
    },
    focus: {
        supportingText: "text-content-muted"
    },
    error: {
        feedbackBackground: "bg-surface-danger-subtle",
        feedbackColor: "text-content-danger",
        supportingText: "text-content-danger"
    },
    success: {
        feedbackBackground: "bg-surface-success-subtle",
        feedbackColor: "text-content-success",
        supportingText: "text-content-success"
    }
};

const feedbackIcons = {
    error: ErrorIcon,
    success: SuccessIcon
};

function TextInput({
    className,
    contentClassName,
    feedbackClassName,
    inputText,
    inputClassName,
    leadingIcon: LeadingIcon = AccountIcon,
    placeholder = "Placeholder",
    showLeadingIcon = true,
    showSupportingText = true,
    showTitle = true,
    state = "default",
    supportingText = "Use 3–20 letters, numbers or underscores.",
    title = "Title",
    value,
    defaultValue = inputText ?? "",
    onChange,
    name,
    type = "text",
    autoComplete,
    disabled = false,
    required = false,
    maxLength,
    id,
    autoFocus = false,
    inputRef,
    "aria-label": ariaLabel
}) {
    const [internalValue, setInternalValue] = useState(defaultValue);
    const generatedId = useId();
    const inputId = id || generatedId;
    const isControlled = value !== undefined;
    const currentValue = isControlled ? value : internalValue;
    const resolvedState = state === "default" && currentValue ? "focus" : state;
    const FeedbackIcon = feedbackIcons[resolvedState];
    const resolvedStyles = stateStyles[resolvedState] || stateStyles.default;

    return (
        <article
            className={twMerge(
                "shadow-elevation-200 grid w-full min-w-0 grid-cols-[minmax(0,1fr)_auto]",
                className
            )}
            data-state={resolvedState}
        >
            <div
                className={twMerge(
                    "flex min-w-0 flex-1 flex-col gap-xs border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel px-md py-xs",
                    contentClassName
                )}
            >
                {showTitle && (
                    <label
                        className="text-body-default-medium text-content-primary"
                        htmlFor={inputId}
                    >
                        {title}
                    </label>
                )}
                <div className="flex min-w-0 items-start gap-xs text-content-primary">
                    {showLeadingIcon && (
                        <span
                            aria-hidden="true"
                            className="size-icon-sm shrink-0"
                        >
                            <LeadingIcon className="size-full" />
                        </span>
                    )}
                    <input
                        ref={inputRef}
                        aria-label={ariaLabel}
                        aria-describedby={
                            showSupportingText
                                ? `${inputId}-supporting-text`
                                : undefined
                        }
                        aria-invalid={resolvedState === "error"}
                        autoComplete={autoComplete}
                        className={clsx(
                            twMerge(
                                "min-w-0 flex-1 overflow-x-auto placeholder:text-content-muted focus:border-0 focus-visible:outline-none disabled:cursor-not-allowed disabled:opacity-60 [scrollbar-width:none] [&::-webkit-scrollbar]:hidden",
                                inputClassName
                            ),
                            "text-body-default"
                        )}
                        disabled={disabled}
                        autoFocus={autoFocus}
                        id={inputId}
                        maxLength={maxLength}
                        name={name}
                        onChange={(event) => {
                            if (!isControlled) {
                                setInternalValue(event.target.value);
                            }
                            onChange?.(event);
                        }}
                        placeholder={placeholder}
                        required={required}
                        type={type}
                        value={currentValue}
                    />
                </div>
                {showSupportingText && (
                    <p
                        id={`${inputId}-supporting-text`}
                        tabIndex={0}
                        onKeyDown={(event) => {
                            const text = event.currentTarget;
                            if (text.scrollWidth <= text.clientWidth) return;
                            if (event.key === "ArrowLeft") {
                                text.scrollLeft -= 40;
                            } else if (event.key === "ArrowRight") {
                                text.scrollLeft += 40;
                            } else if (event.key === "Home") {
                                text.scrollLeft = 0;
                            } else if (event.key === "End") {
                                text.scrollLeft = text.scrollWidth;
                            } else {
                                return;
                            }
                            event.preventDefault();
                        }}
                        className={clsx(
                            "text-body-sm min-w-0 overflow-x-auto border-0 whitespace-nowrap focus-visible:underline focus-visible:outline-none [scrollbar-width:none] [&::-webkit-scrollbar]:hidden",
                            resolvedStyles.supportingText
                        )}
                    >
                        {supportingText}
                    </p>
                )}
            </div>
            {FeedbackIcon && (
                <div
                    aria-hidden="true"
                    className={twMerge(
                        clsx(
                            "grid aspect-square box-border shrink-0 place-items-center border-[length:var(--border-width-default)] border-l-0 border-stroke-default p-xs",
                            resolvedStyles.feedbackBackground,
                            resolvedStyles.feedbackColor,
                            feedbackClassName
                        )
                    )}
                >
                    <span
                        className="aspect-square h-full bg-current"
                        style={{
                            maskImage: `url("${FeedbackIcon}")`,
                            maskPosition: "center",
                            maskRepeat: "no-repeat",
                            maskSize: "contain"
                        }}
                    />
                </div>
            )}
        </article>
    );
}

export default TextInput;
