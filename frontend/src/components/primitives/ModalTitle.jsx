import { twMerge } from "tailwind-merge";

function ModalTitle({ title = "TITLE", children, as = "h2", id, className }) {
    const Content = children ?? title;
    const Tag = as;

    return (
        <Tag
            id={id}
            className={twMerge(
                "text-heading-xl w-full text-center text-content-primary",
                className
            )}
        >
            {Content}
        </Tag>
    );
}

export default ModalTitle;
