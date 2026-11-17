const assetUrls = Object.values(
    import.meta.glob("./assets/**/*", {
        eager: true,
        import: "default",
        query: "?url"
    })
);

let startupManagerPromise;

export function connectToStartupManager() {
    if (startupManagerPromise) return startupManagerPromise;

    if (
        !window.qt?.webChannelTransport ||
        typeof window.QWebChannel !== "function"
    ) {
        return Promise.resolve(null);
    }

    startupManagerPromise = new Promise((resolve, reject) => {
        new window.QWebChannel(window.qt.webChannelTransport, (channel) => {
            const manager = channel.objects.startupManager;
            if (!manager) {
                reject(
                    new Error("Không tìm thấy dịch vụ khởi tạo của ứng dụng.")
                );
                return;
            }
            resolve(manager);
        });
    });

    return startupManagerPromise;
}

export async function initializeDatabase(manager) {
    if (!manager) return;

    if (manager.ready) return;

    await new Promise((resolve, reject) => {
        const handleFinished = (success, message) => {
            manager.initializationFinished.disconnect(handleFinished);
            if (success) {
                resolve();
            } else {
                reject(
                    new Error(
                        message || "Không thể kết nối cơ sở dữ liệu SQLite."
                    )
                );
            }
        };

        manager.initializationFinished.connect(handleFinished);
        manager.initialize();
    });
}

export async function verifyResourceIntegrity() {
    const results = await Promise.all(
        assetUrls.map(async (url) => {
            const response = await fetch(url);
            if (!response.ok) {
                throw new Error(
                    `Không thể đọc tài nguyên ứng dụng (${response.status}).`
                );
            }

            const content = await response.arrayBuffer();
            if (content.byteLength === 0) {
                throw new Error("Phát hiện tệp tài nguyên ứng dụng bị rỗng.");
            }
        })
    );

    return results;
}

export async function loadApplicationFonts() {
    const fonts = [
        '400 16px "Space Grotesk"',
        '500 16px "Space Grotesk"',
        '700 16px "Space Grotesk"',
        '400 14px "Space Mono"',
        '700 14px "Space Mono"'
    ];

    await Promise.all(fonts.map((font) => document.fonts.load(font)));

    const missingFont = fonts.find((font) => !document.fonts.check(font));
    if (missingFont) {
        throw new Error(`Không thể nạp font chữ: ${missingFont}.`);
    }
}
