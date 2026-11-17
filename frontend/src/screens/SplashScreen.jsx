import BrandLogo from "@/components/BrandLogo.jsx";

function SplashScreen({ error, onRetry, progress, status }) {
    return (
        <main className="flex min-h-screen items-center justify-center bg-surface-canvas px-md py-2xl font-sans text-content-primary">
            <section
                aria-label="Đang khởi động WordSprint"
                aria-live="polite"
                className="w-full max-w-[440px] border-2 border-stroke-default bg-surface-panel px-xl py-2xl text-center shadow-elevation-400 sm:px-2xl"
            >
                <div className="flex justify-center">
                    <BrandLogo />
                </div>

                <p className="text-mono-badge mb-xs mt-xl text-content-muted">
                    VOCABULARY ARCADE
                </p>
                <h1 className="text-heading-xl m-0">Bắt đầu hành trình</h1>
                <p className="text-body-default mb-xl mt-xs text-content-muted">
                    WordSprint đang chuẩn bị không gian luyện từ vựng cho bạn.
                </p>

                <div className="mb-sm flex items-center justify-between gap-md text-left">
                    <span className="text-body-default-medium">{status}</span>
                    <span className="text-mono-data shrink-0">{progress}%</span>
                </div>
                <div
                    className="h-[14px] overflow-hidden border-2 border-stroke-default bg-surface-canvas"
                    role="progressbar"
                    aria-label="Tiến trình khởi động"
                    aria-valuemin={0}
                    aria-valuemax={100}
                    aria-valuenow={progress}
                >
                    <div
                        className="h-full bg-surface-primary-cta transition-[width] duration-300 ease-snap"
                        style={{ width: `${progress}%` }}
                    />
                </div>

                {error ? (
                    <div className="mt-lg text-left" role="alert">
                        <p className="text-body-default-bold mb-xs text-content-danger">
                            Khởi động chưa hoàn tất
                        </p>
                        <p className="text-body-default m-0 text-content-muted">
                            {error}
                        </p>
                        <button
                            className="text-button-default mt-md border-2 border-stroke-default bg-surface-primary-cta px-md py-xs shadow-elevation-100"
                            onClick={onRetry}
                            type="button"
                        >
                            Thử lại
                        </button>
                    </div>
                ) : (
                    <p className="text-body-sm mt-lg text-content-muted">
                        Đang kiểm tra kết nối, font chữ và tài nguyên...
                    </p>
                )}
            </section>
        </main>
    );
}

export default SplashScreen;
