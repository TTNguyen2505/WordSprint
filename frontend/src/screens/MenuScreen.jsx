import BrandLogo from "@/components/BrandLogo.jsx";

const learningModes = [
    { code: "01", label: "Xáo trộn chữ", detail: "Sắp xếp chữ cái thành từ" },
    {
        code: "02",
        label: "Điền từ còn thiếu",
        detail: "Hoàn thành từ theo gợi ý"
    },
    { code: "03", label: "Nghe và đoán", detail: "Luyện nghe phát âm" }
];

function MenuScreen() {
    return (
        <main className="min-h-screen bg-surface-canvas px-md py-lg font-sans text-content-primary sm:px-2xl">
            <header className="mx-auto flex w-full max-w-[1080px] items-center justify-between">
                <BrandLogo />
                <span className="text-mono-badge border-2 border-stroke-default bg-surface-success-subtle px-xs py-2xs text-content-success">
                    SẴN SÀNG
                </span>
            </header>

            <section className="mx-auto grid min-h-[calc(100vh-120px)] w-full max-w-[1080px] items-center gap-xl py-2xl md:grid-cols-[1.1fr_0.9fr]">
                <div className="flex flex-col items-start">
                    <p className="text-mono-badge mb-sm border-2 border-stroke-default bg-surface-primary-cta px-xs py-2xs">
                        HỌC TỪ VỰNG · CHƠI MỖI NGÀY
                    </p>
                    <h1 className="text-display-hero m-0 max-w-[640px]">
                        Mỗi từ mới,
                        <br />
                        một bước tiến.
                    </h1>
                    <p className="text-subtitle-default mt-md max-w-[520px] text-content-muted">
                        Rèn vốn từ tiếng Anh qua những thử thách ngắn, vui và
                        đầy bất ngờ.
                    </p>
                    <div className="mt-xl flex items-center gap-sm border-2 border-stroke-default bg-surface-panel px-md py-sm shadow-elevation-200">
                        <span
                            aria-hidden="true"
                            className="grid size-icon-md place-items-center bg-surface-success text-content-primary"
                        >
                            ✓
                        </span>
                        <div>
                            <p className="text-body-default-bold m-0">
                                Hệ thống đã sẵn sàng
                            </p>
                            <p className="text-body-sm m-0 text-content-muted">
                                Tài nguyên và font chữ đã được kiểm tra.
                            </p>
                        </div>
                    </div>
                </div>

                <div className="flex flex-col gap-sm">
                    <h2 className="text-heading-md m-0 mb-2xs">
                        Chọn thử thách của bạn
                    </h2>
                    {learningModes.map(({ code, label, detail }) => (
                        <article
                            key={code}
                            className="flex items-center gap-md border-2 border-stroke-default bg-surface-panel p-md shadow-elevation-100"
                        >
                            <span className="text-mono-data grid size-[44px] shrink-0 place-items-center border-2 border-stroke-default bg-surface-primary-cta">
                                {code}
                            </span>
                            <div>
                                <h3 className="text-heading-sm m-0">{label}</h3>
                                <p className="text-body-default m-0 mt-2xs text-content-muted">
                                    {detail}
                                </p>
                            </div>
                        </article>
                    ))}
                    <p className="text-body-sm m-0 mt-xs text-content-muted">
                        Các chế độ chơi sẽ sớm có mặt.
                    </p>
                </div>
            </section>
        </main>
    );
}

export default MenuScreen;
