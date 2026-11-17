function BrandLogo({ compact = false }) {
    return (
        <div className="flex items-center gap-sm" aria-label="WordSprint">
            <span
                aria-hidden="true"
                className="grid size-[52px] shrink-0 place-items-center border-2 border-stroke-default bg-surface-primary-cta text-heading-lg shadow-elevation-200"
            >
                W
            </span>
            {!compact && (
                <span className="text-heading-lg tracking-[-0.04em]">
                    WordSprint
                </span>
            )}
        </div>
    );
}

export default BrandLogo;
