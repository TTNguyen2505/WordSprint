import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";
import TextButton from "./TextButton";
import UserIcon from "../assets/icons/user.svg?react";

/**
 * Neo‑Brutalism ProfileCard component.
 *
 * Props:
 *   - label (string) – top label above the username (default: "CURRENT PLAYER").
 *   - username (string) – displayed username (default: "TTNguyen2505").
 *   - actionLabel (string) – text for the inline action button (default: "Switch Profile").
 *   - onSwitchProfile (function) – click handler for the action button.
 *   - icon (ReactNode) – optional custom user icon; falls back to the default outline user circle.
 *   - className (string) – optional extra Tailwind classes for the outer card.
 */
function ProfileCard({
  label = "CURRENT PLAYER",
  username = "TTNguyen2505",
  actionLabel = "Switch Profile",
  onSwitchProfile,
  icon,
  className,
}) {
  const handleClick = (e) => {
    if (typeof onSwitchProfile === "function") {
      onSwitchProfile(e);
    }
  };

  return (
    <div
      className={twMerge(
        clsx(
          "flex items-center justify-between gap-3 rounded-none border-2 border-black",
          "shadow-elevation-200 bg-white p-4",
          "max-w-[320px] w-full",
          className
        )
      )}
    >
      {/* Left side: icon badge + text */}
      <div className="flex items-center gap-3">
        {/* Icon badge */}
        <div className="flex shrink-0 items-center justify-center w-11 h-11 rounded-none border-2 border-black bg-surface-primary-cta shadow-elevation-200">
          {icon ?? <UserIcon className="size-[22px] text-black" aria-hidden="true" />}
        </div>
        {/* Text column */}
        <div className="flex flex-col gap-0.5">
          <span className="text-[11px] font-mono font-bold uppercase tracking-wider text-black">
            {label}
          </span>
          <span className="text-sm font-mono font-medium text-black whitespace-nowrap">
            {username}
          </span>
        </div>
      </div>

      {/* Right side: inline TextButton */}
      <TextButton
        label={actionLabel}
        onClick={handleClick}
        className="self-end"
      />
    </div>
  );
}

export default ProfileCard;
