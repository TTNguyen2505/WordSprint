import clsx from "clsx";
import { twMerge } from "tailwind-merge";
import React from "react";

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
function ModalTitle({
  title = "TITLE",
  children,
  as = "h2",
  id,
  className,
}) {
  const Content = children ?? title;
  const Tag = as;

  const baseClasses = clsx(
    "font-sans font-bold uppercase tracking-tight text-content-primary",
    "bg-transparent m-0",
    "text-[length:var(--primitive-font-size-36)] leading-[var(--primitive-line-height-40)]"
  );

  return (
    <Tag id={id} className={twMerge(baseClasses, className)}>
      {Content}
    </Tag>
  );
}

export default ModalTitle;
