import boltIcon from "@/assets/icons/bolt.svg?url";

function SplashScreen({ error, onRetry, progress, status }) {
    return (
        <main className="splash-screen">
            <div className="splash-grid" aria-hidden="true" />
            <section className="splash-content" aria-labelledby="splash-title">
                <div className="splash-brand-mark" aria-hidden="true">
                    <img src={boltIcon} alt="" />
                </div>
                <p className="splash-eyebrow">VOCABULARY PUZZLE GAME</p>
                <h1 id="splash-title" className="splash-title">
                    Word<span>Sprint</span>
                </h1>
                <p className="splash-tagline">Learn words. Sprint ahead.</p>

                <div
                    className="splash-progress"
                    role="progressbar"
                    aria-label="Tiến trình khởi động"
                    aria-valuemin={0}
                    aria-valuemax={100}
                    aria-valuenow={progress}
                >
                    <span style={{ width: `${progress}%` }} />
                </div>
                <p className="splash-status" aria-live="polite">
                    {error || status}
                </p>

                {error ? (
                    <button className="splash-retry" onClick={onRetry}>
                        Thử lại
                    </button>
                ) : (
                    <div className="splash-loader" aria-hidden="true" />
                )}
            </section>
            <p className="splash-footer">A LITTLE PROGRESS, EVERY DAY.</p>
        </main>
    );
}

export default SplashScreen;
