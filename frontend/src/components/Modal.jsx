import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import { useEffect, useState } from "react";

function Modal({
    isOpen = true,
    onClose,
    children,
    header,
    body,
    footer,
    title,
    showCloseButton = false,
    closeOnBackdropClick = true,
    closeOnEscape = true,
    maxWidth = "max-w-[752px]",
    className,
    overlayClassName,
    "aria-labelledby": ariaLabelledBy
}) {
    const [shouldRender, setShouldRender] = useState(isOpen);
    const [isClosing, setIsClosing] = useState(false);

    useEffect(() => {
        if (isOpen) {
            const openTimeoutId = window.setTimeout(() => {
                setShouldRender(true);
                setIsClosing(false);
            }, 0);
            return () => window.clearTimeout(openTimeoutId);
        }

        if (!shouldRender) return undefined;

        const closeTimeoutId = window.setTimeout(() => {
            setIsClosing(true);
        }, 0);
        const timeoutId = window.setTimeout(() => {
            setShouldRender(false);
            setIsClosing(false);
        }, 240);

        return () => {
            window.clearTimeout(closeTimeoutId);
            window.clearTimeout(timeoutId);
        };
    }, [isOpen, shouldRender]);

    useEffect(() => {
        if (!shouldRender) return undefined;
        const originalOverflow = document.body.style.overflow;
        document.body.style.overflow = "hidden";
        return () => {
            document.body.style.overflow = originalOverflow;
        };
    }, [shouldRender]);

    useEffect(() => {
        if (!isOpen || !closeOnEscape) return undefined;
        const handler = (e) => {
            if (e.key === "Escape" && typeof onClose === "function") {
                onClose();
            }
        };
        window.addEventListener("keydown", handler);
        return () => window.removeEventListener("keydown", handler);
    }, [isOpen, closeOnEscape, onClose]);

    if (!shouldRender) return null;

    const handleBackdropClick = () => {
        if (closeOnBackdropClick && typeof onClose === "function") {
            onClose();
        }
    };

    const stopPropagation = (e) => e.stopPropagation();

    return (
        <div
            className={twMerge(
                clsx(
                    "fixed inset-0 z-50 flex items-center justify-center bg-black/50 p-md backdrop-blur-20",
                    isClosing
                        ? "animate-[modal-backdrop-exit_var(--duration-deliberate)_var(--ease-snap)_both]"
                        : "animate-[modal-backdrop-enter_var(--duration-deliberate)_var(--ease-snap)_both]",
                    "motion-reduce:animate-none",
                    overlayClassName
                )
            )}
            onClick={handleBackdropClick}
        >
            <section
                role="dialog"
                aria-modal="true"
                aria-labelledby={ariaLabelledBy}
                className={twMerge(
                    clsx(
                        "flex w-full flex-col items-center justify-center gap-3xl rounded-none border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel px-2xl py-xl shadow-elevation-200",
                        isClosing
                            ? "animate-[modal-exit_var(--duration-deliberate)_var(--ease-release)_both]"
                            : "animate-[modal-enter_var(--duration-deliberate)_var(--ease-release)_both]",
                        "motion-reduce:animate-none",
                        maxWidth,
                        className
                    )
                )}
                onClick={stopPropagation}
            >
                {(header || title || showCloseButton) && (
                    <header className="flex w-full shrink-0 items-center justify-between">
                        {header}
                        {title && (
                            <h2 className="text-heading-lg text-content-primary">
                                {title}
                            </h2>
                        )}
                        {showCloseButton && (
                            <button
                                type="button"
                                aria-label="Close modal"
                                onClick={onClose}
                                className={twMerge(
                                    "inline-flex size-icon-md items-center justify-center rounded-none border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel text-content-primary shadow-elevation-200 transition-[translate,box-shadow] duration-[var(--duration-normal)] ease-snap hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] active:translate-x-[var(--primitive-depth-100)] active:translate-y-[var(--primitive-depth-100)] active:shadow-elevation-50"
                                )}
                            >
                                &times;
                            </button>
                        )}
                    </header>
                )}
                {body || (
                    <div className="w-fit max-w-full shrink-0">{children}</div>
                )}
                {footer && (
                    <footer className="flex w-full shrink-0 items-center justify-center">
                        {footer}
                    </footer>
                )}
            </section>
        </div>
    );
}

export default Modal;
