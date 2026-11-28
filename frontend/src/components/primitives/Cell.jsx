import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import DeleteIcon from "../../assets/icons/delete.svg?react";
import Badge from "./Badge";
import IconButton from "./IconButton";

const legacyTypes = {
    default: "Text",
    bold: "Header",
    badge: "Text",
    muted: "Numerical",
    action: "Action"
};

function Cell({
    type,
    variant,
    data,
    value = "01",
    children,
    showBadge = true,
    badgeLabel = "BADGE TITLE",
    badgeVariant = "black",
    badgeProps = {},
    icon,
    onAction,
    actionLabel = "Delete",
    as = "div",
    role,
    className
}) {
    const cellType = type ?? legacyTypes[variant] ?? "Header";
    const displayValue = data ?? children ?? value;
    const shouldShowBadge =
        variant === undefined ? showBadge : variant === "badge";
    const Wrapper = as === "td" ? "td" : "div";

    const widthClass =
        {
            Header: "w-[91px] justify-center",
            Rank: "w-[91px] justify-center",
            Action: "w-[91px] justify-center",
            Numerical: "w-[34px] justify-end",
            Text: "w-[180px] gap-xs"
        }[cellType] ?? "w-[91px] justify-center";

    const content = (() => {
        if (children) return children;

        if (cellType === "Action") {
            return (
                <IconButton
                    ariaLabel={actionLabel}
                    onClick={onAction}
                    icon={
                        icon ?? (
                            <DeleteIcon
                                className="size-full text-current"
                                aria-hidden="true"
                            />
                        )
                    }
                />
            );
        }

        if (cellType === "Header") {
            return <span className="text-heading-sm">{displayValue}</span>;
        }

        if (cellType === "Text") {
            return (
                <>
                    <span className="text-mono-data">{displayValue}</span>
                    {shouldShowBadge && (
                        <Badge variant={badgeVariant} showIcon {...badgeProps}>
                            {badgeLabel}
                        </Badge>
                    )}
                </>
            );
        }

        return <span className="text-mono-data">{displayValue}</span>;
    })();

    return (
        <Wrapper
            role={role}
            className={twMerge(
                clsx(
                    "inline-flex h-[var(--primitive-space-48)] shrink-0 items-center px-xs",
                    widthClass,
                    cellType === "Numerical" && "text-right",
                    cellType === "Text" && "overflow-hidden",
                    className
                )
            )}
        >
            {content}
        </Wrapper>
    );
}

export default Cell;
