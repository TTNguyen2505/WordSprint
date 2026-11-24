import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import Tab from "./Tab";

function Navigation({
    items = ["Case 1", "Case 2", "Case 3", "Case 4", "Overall"],
    activeTab = "Overall",
    onChange,
    navigationRef,
    className
}) {
    const normalized = items.map((it) => {
        if (typeof it === "string") {
            return { label: it, value: it };
        }
        return { label: it.label, value: it.value ?? it.label };
    });

    const handleSelect = (value) => {
        if (typeof onChange === "function") {
            onChange(value);
        }
    };

    return (
        <nav
            ref={navigationRef}
            role="tablist"
            className={twMerge(
                clsx(
                    "flex w-[652px] max-w-full items-start gap-2xs overflow-x-auto bg-surface-panel px-md py-xs [scrollbar-width:none] [&::-webkit-scrollbar]:hidden",
                    "border-[length:var(--border-width-default)] border-stroke-default",
                    "shadow-elevation-200",
                    className
                )
            )}
        >
            {normalized.map((item) => (
                <Tab
                    key={item.value}
                    label={item.label}
                    active={item.value === activeTab}
                    onClick={() => handleSelect(item.value)}
                    className="w-[120px] shrink-0 self-stretch"
                />
            ))}
        </nav>
    );
}

export default Navigation;
