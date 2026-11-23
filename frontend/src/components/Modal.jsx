import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import { useEffect } from "react";

/**
 * Neo‑Brutalism Modal component.
 * Renders a dimmed backdrop and a centered dialog with sharp corners,
 * a solid 1px black border and a hard bottom‑right offset shadow.
 *
 * Props:
 *   - isOpen (boolean) – controls visibility; when false the component returns null.
 *   - onClose (function) – called when the modal should close (backdrop click, Escape key, close button).
 *   - children (ReactNode) – content rendered inside the dialog box.
 *   - title (string) – optional header title; renders a header bar when provided.
 *   - showCloseButton (boolean) – whether to display a Neo‑Brutalism close button (default false).
 *   - closeOnBackdropClick (boolean) – close when clicking the backdrop (default true).
 *   - closeOnEscape (boolean) – close on Escape key press (default true).
 *   - maxWidth (string) – Tailwind max‑width utility for the dialog (default "max-w-[680px]").
 *   - className (string) – additional classes for the dialog container.
 *   - overlayClassName (string) – additional classes for the backdrop overlay.
 */
function Modal({
    isOpen = true,
    onClose,
    children,
    title,
    showCloseButton = false,
    closeOnBackdropClick = true,
    closeOnEscape = true,
    maxWidth = "max-w-[680px]",
    className,
    overlayClassName
}) {
    // Prevent background scroll while the modal is open
    useEffect(() => {
        if (isOpen) {
            const originalOverflow = document.body.style.overflow;
            document.body.style.overflow = "hidden";
            return () => {
                document.body.style.overflow = originalOverflow;
            };
        }
    }, [isOpen]);

    // Escape key handling
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

    if (!isOpen) return null;

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
                    "fixed inset-0 z-50 flex items-center justify-center p-md sm:p-lg bg-black/50",
                    overlayClassName
                )
            )}
            onClick={handleBackdropClick}
        >
            <section
                role="dialog"
                aria-modal="true"
                className={twMerge(
                    clsx(
                        "w-full",
                        maxWidth,
                        "min-h-[360px] max-h-[90vh] overflow-y-auto",
                        "bg-surface-panel border border-stroke-default rounded-none",
                        "shadow-elevation-200",
                        "p-lg",
                        className
                    )
                )}
                onClick={stopPropagation}
            >
                {/* Optional header */}
                {(title || showCloseButton) && (
                    <header className="flex items-center justify-between mb-md">
                        {title && (
                            <h2 className="font-bold text-content-primary text-lg">
                                {title}
                            </h2>
                        )}
                        {showCloseButton && (
                            <button
                                type="button"
                                aria-label="Close modal"
                                onClick={onClose}
                                className={twMerge(
                                    clsx(
                                        "w-7 h-7 bg-surface-panel border border-stroke-default rounded-none",
                                        "shadow-elevation-200",
                                        "inline-flex items-center justify-center text-content-primary",
                                        "cursor-pointer hover:-translate-x-[1px] hover:-translate-y-[1px] active:translate-x-[2px] active:translate-y-[2px] active:shadow-none transition-all"
                                    )
                                )}
                            >
                                &times;
                            </button>
                        )}
                    </header>
                )}
                {/* Main content */}
                {children}
            </section>
        </div>
    );
}

export default Modal;
