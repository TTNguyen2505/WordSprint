import clsx from "clsx";
import { twMerge } from "tailwind-merge";

function ModalTitle({ title = "TITLE", children, as = "h2", id, className }) {
    const Content = children ?? title;
    const Tag = as;

    const baseClasses = clsx(
        "font-sans text-[length:var(--primitive-font-size-24)] font-bold leading-[var(--primitive-line-height-30)] tracking-[var(--primitive-letter-spacing-tight)] text-content-primary"
    );

    return (
        <Tag id={id} className={twMerge(baseClasses, className)}>
            {Content}
        </Tag>
    );
}

export default ModalTitle;
