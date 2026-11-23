import clsx from "clsx";
import { twMerge } from "tailwind-merge";

/**
 * Neo‑Brutalism ModalTitle component.
 *
 * Props:
 *   - title (string) – fallback heading text, default "TITLE".
 *   - children (ReactNode) – content takes precedence over `title`.
 *   - as (string) – HTML tag to render (h1 | h2 | h3 | h4 | div), default "h2".
 *   - id (string) – optional id for aria‑labelling.
 *   - className (string) – optional extra Tailwind classes.
 */
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
