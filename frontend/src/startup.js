const resourceUrls = Object.values(
    import.meta.glob("./assets/**/*", {
        eager: true,
        import: "default",
        query: "?url"
    })
);

const fontFaces = [
    '400 16px "Space Grotesk"',
    '500 16px "Space Grotesk"',
    '700 16px "Space Grotesk"',
    '400 16px "Space Mono"',
    '700 16px "Space Mono"'
];

let desktopChannelPromise;

function initializeDatabase() {
    const transport = window.qt?.webChannelTransport;
    if (!transport || typeof window.QWebChannel !== "function") {
        return Promise.resolve({
            available: false,
            message: "SQLite chỉ khả dụng trong ứng dụng WordSprint Desktop."
        });
    }

    desktopChannelPromise ??= new Promise((resolve) => {
        new window.QWebChannel(transport, resolve);
    });

    return desktopChannelPromise.then(
        (channel) =>
            new Promise((resolve, reject) => {
                const databaseManager = channel.objects.databaseManager;
                if (!databaseManager?.initialize) {
                    reject(
                        new Error("Không tìm thấy dịch vụ khởi tạo SQLite.")
                    );
                    return;
                }

                databaseManager.initialize((result) => {
                    if (!result?.success) {
                        reject(
                            new Error(
                                result?.error || "Không thể khởi tạo SQLite."
                            )
                        );
                        return;
                    }

                    resolve({ available: true });
                });
            })
    );
}

async function loadFonts() {
    await Promise.all(
        fontFaces.map(async (descriptor) => {
            const loadedFaces = await document.fonts.load(descriptor);
            if (loadedFaces.length === 0) {
                throw new Error(`Không thể nạp font: ${descriptor}.`);
            }
        })
    );
}

async function verifyResources(onProgress) {
    if (resourceUrls.length === 0) {
        throw new Error("Không tìm thấy tài nguyên trong thư mục assets.");
    }

    let verifiedCount = 0;
    await Promise.all(
        resourceUrls.map(async (url) => {
            const response = await fetch(url);
            if (!response.ok) {
                throw new Error(`Không thể tải tài nguyên: ${url}.`);
            }

            const content = await response.arrayBuffer();
            if (content.byteLength === 0) {
                throw new Error(`Tài nguyên bị trống: ${url}.`);
            }

            verifiedCount += 1;
            onProgress(verifiedCount / resourceUrls.length);
        })
    );
}

export async function initializeApplication(onProgress) {
    const startedAt = Date.now();
    onProgress({ progress: 10, status: "Đang kết nối cơ sở dữ liệu..." });
    const database = await initializeDatabase();

    onProgress({ progress: 35, status: "Đang nạp phông chữ..." });
    await loadFonts();

    onProgress({ progress: 60, status: "Đang kiểm tra tài nguyên..." });
    await verifyResources((fraction) => {
        onProgress({
            progress: 60 + Math.round(fraction * 40),
            status: "Đang kiểm tra tài nguyên..."
        });
    });

    await new Promise((resolve) =>
        window.setTimeout(resolve, Math.max(0, 600 - (Date.now() - startedAt)))
    );

    return database;
}
