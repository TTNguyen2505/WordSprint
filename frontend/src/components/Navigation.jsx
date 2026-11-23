import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Tab from "./Tab";

/**
 * Neo‑Brutalism Navigation bar component.
 * Wraps a set of `Tab` items inside a bordered white container with a hard offset shadow.
 *
 * Props:
 *   - items (array) – strings or objects `{label, value, disabled?}`.
 *       Default: ['Case 1', 'Case 2', 'Case 3', 'Case 4', 'Overall']
 *   - activeTab (string) – currently selected tab value/label. Default: 'Overall'.
 *   - onChange (function) – called with the selected value when a tab is clicked.
 *   - className (string) – optional extra Tailwind classes for the outer container.
 */
function Navigation({
    items = ["Case 1", "Case 2", "Case 3", "Case 4", "Overall"],
    activeTab = "Overall",
    onChange,
    className
}) {
    // Normalise items to { label, value, disabled }
    const normalized = items.map((it) => {
        if (typeof it === "string") {
            return { label: it, value: it, disabled: false };
        }
        return {
            label: it.label,
            value: it.value ?? it.label,
            disabled: !!it.disabled
        };
    });

    const handleSelect = (value) => {
        if (typeof onChange === "function") {
            onChange(value);
        }
    };

    return (
        <nav
            role="tablist"
            className={twMerge(
                clsx(
                    "flex w-full items-center gap-1.5 p-1.5",
                    "bg-surface-panel border border-stroke-default",
                    // Hard offset shadow (bottom‑right)
                    "shadow-elevation-200",
                    className
                )
            )}
        >
            {normalized.map((item) => (
                <Tab
                    key={item.value}
                    label={item.label}
                    active={item.value === activeTab && !item.disabled}
                    disabled={item.disabled}
                    onClick={() => handleSelect(item.value)}
                    className="flex-1 h-10"
                />
            ))}
        </nav>
    );
}

export default Navigation;
