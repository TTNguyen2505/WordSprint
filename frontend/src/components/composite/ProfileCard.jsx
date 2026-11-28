import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import TextButton from "../primitives/TextButton";
import UserIcon from "../../assets/icons/account.svg?react";

function ProfileCard({
    label = "CURRENT PLAYER",
    username = "TTNguyen2505",
    actionLabel = "Switch Profile",
    onSwitchProfile,
    icon,
    state = "Default",
    className
}) {
    const isHover = state === "Hover" || state === "hover";

    const handleClick = (e) => {
        if (typeof onSwitchProfile === "function") {
            onSwitchProfile(e);
        }
    };

    return (
        <div
            className={twMerge(
                clsx(
                    "inline-flex w-fit max-w-full items-center gap-md rounded-none border-[length:var(--border-width-default)] border-stroke-default bg-surface-panel p-md shadow-elevation-200 transition-[translate,box-shadow] duration-[var(--duration-normal)] ease-snap hover:-translate-x-[var(--primitive-depth-50)] hover:-translate-y-[var(--primitive-depth-50)] hover:shadow-elevation-300",
                    isHover && "shadow-elevation-300",
                    className
                )
            )}
        >
            <div className="flex size-[var(--primitive-space-48)] shrink-0 items-center justify-center rounded-none border-[length:var(--border-width-default)] border-stroke-default bg-surface-primary-cta p-xs shadow-elevation-100">
                {icon ?? (
                    <UserIcon
                        className="text-content-primary"
                        aria-hidden="true"
                    />
                )}
            </div>
            <div className="flex shrink-0 flex-col gap-xs text-content-primary">
                <span className="text-mono-badge">{label}</span>
                <div className="flex items-center gap-lg">
                    <span className="text-mono-data whitespace-nowrap">
                        {username}
                    </span>
                    <TextButton label={actionLabel} onClick={handleClick} />
                </div>
            </div>
        </div>
    );
}

export default ProfileCard;
