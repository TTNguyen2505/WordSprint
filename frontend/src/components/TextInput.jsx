import clsx from "clsx";
import { useId, useState } from "react";
import { twMerge } from "tailwind-merge";
import AccountIcon from "../assets/icons/account.svg?react";
import ErrorIcon from "../assets/icons/wrong.svg?url";
import SuccessIcon from "../assets/icons/check.svg?url";

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
        content: "p-xs",
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
    const compactFeedback = !showTitle && !showSupportingText && FeedbackIcon;

    return (
        <article
            className={twMerge(
                clsx(
                    "shadow-elevation-200 grid min-w-0 w-full",
                    compactFeedback
                        ? "grid-cols-[minmax(0,1fr)_auto]"
                        : "border-2 border-stroke-default bg-surface-panel",
                    FeedbackIcon
                        ? "grid-cols-[minmax(0,1fr)_auto]"
                        : "grid-cols-1"
                ),
                className
            )}
            data-state={resolvedState}
        >
            <div
                className={twMerge(
                    clsx(
                        "flex min-w-[0px] flex-1 flex-col gap-x-xs gap-y-[6px]",
                        compactFeedback
                            ? "border-2 border-stroke-default bg-surface-panel px-md py-[6px]"
                            : "px-md py-xs",
                        !compactFeedback && resolvedStyles.content
                    ),
                    contentClassName
                )}
            >
                {showTitle && (
                    <label
                        className="text-body-default-medium m-0 text-content-primary"
                        htmlFor={inputId}
                    >
                        {title}
                    </label>
                )}
                <div className="text-body-default flex min-w-[0px] items-start gap-xs text-content-primary">
                    {showLeadingIcon && (
                        <span
                            aria-hidden="true"
                            className="size-icon-sm shrink-0"
                        >
                            <LeadingIcon className="size-full shrink-0 [&>rect]:hidden" />
                        </span>
                    )}
                    <input
                        aria-label={ariaLabel}
                        aria-describedby={
                            showSupportingText
                                ? `${inputId}-supporting-text`
                                : undefined
                        }
                        aria-invalid={resolvedState === "error"}
                        autoComplete={autoComplete}
                        className={twMerge(
                            clsx(
                                "text-body-default min-w-[0px] flex-1 border-0 bg-transparent p-[0px] text-content-primary outline-none placeholder:text-content-muted disabled:cursor-not-allowed disabled:opacity-60",
                                !currentValue &&
                                    "placeholder:text-content-muted"
                            ),
                            inputClassName
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
                        className={clsx(
                            "text-body-sm m-0",
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
                            "shadow-elevation-200 grid aspect-square min-w-0 shrink-0 place-items-center self-stretch",
                            compactFeedback
                                ? "border-2 border-stroke-default p-[6px]"
                                : "border-l-2 border-l-stroke-default p-xs",
                            resolvedStyles.feedbackBackground,
                            resolvedStyles.feedbackColor,
                            feedbackClassName
                        )
                    )}
                >
                    <span
                        className={clsx(
                            compactFeedback
                                ? "size-full min-h-0 min-w-0"
                                : "size-[var(--primitive-space-64)]",
                            "max-h-[var(--primitive-space-64)] max-w-[var(--primitive-space-64)] bg-current"
                        )}
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
