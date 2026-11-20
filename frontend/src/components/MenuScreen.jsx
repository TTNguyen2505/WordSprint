import boltIcon from "@/assets/icons/bolt.svg?url";

function MenuScreen({ databaseAvailable, databaseMessage }) {
    return (
        <main className="menu-screen">
            <header className="menu-header">
                <div className="menu-brand" aria-label="WordSprint">
                    <img src={boltIcon} alt="" />
                    <span>WordSprint</span>
                </div>
                <span className="menu-version">VOCABULARY PUZZLE GAME</span>
            </header>

            <section className="menu-hero" aria-labelledby="menu-title">
                <p className="menu-eyebrow">READY, SET, LEARN</p>
                <h1 id="menu-title">
                    Every word
                    <br />
                    <span>takes you further.</span>
                </h1>
                <p className="menu-description">
                    Build your vocabulary one quick challenge at a time.
                </p>
                <div className="menu-coming-soon">
                    <span className="menu-bolt" aria-hidden="true">
                        <img src={boltIcon} alt="" />
                    </span>
                    <div>
                        <strong>Your next sprint starts soon.</strong>
                        <p>Game modes are being prepared. Stay tuned!</p>
                    </div>
                    <span className="menu-status">COMING SOON</span>
                </div>
            </section>

            {!databaseAvailable && (
                <p className="menu-runtime-notice" role="status">
                    {databaseMessage}
                </p>
            )}

            <footer className="menu-footer">
                <span>WORDSPRINT</span>
                <span>LEARN WORDS. SPRINT AHEAD.</span>
            </footer>
        </main>
    );
}

export default MenuScreen;
