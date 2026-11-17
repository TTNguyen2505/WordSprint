import { useEffect, useState } from "react";
import MenuScreen from "@/screens/MenuScreen.jsx";
import SplashScreen from "@/screens/SplashScreen.jsx";
import {
    connectToStartupManager,
    initializeDatabase,
    loadApplicationFonts,
    verifyResourceIntegrity
} from "@/startup.js";

const startupTasks = [
    { label: "Kết nối SQLite", run: initializeDatabase },
    { label: "Kiểm tra tài nguyên", run: verifyResourceIntegrity },
    { label: "Nạp font chữ", run: loadApplicationFonts }
];

function App() {
    const [screen, setScreen] = useState("splash");
    const [progress, setProgress] = useState(0);
    const [status, setStatus] = useState("Đang chuẩn bị khởi động...");
    const [error, setError] = useState("");
    const [attempt, setAttempt] = useState(0);

    useEffect(() => {
        let cancelled = false;

        async function startApplication() {
            setScreen("splash");
            setProgress(0);
            setError("");

            try {
                const startupManager = await connectToStartupManager();
                let completedTasks = 0;
                const updateProgress = (label) => {
                    if (cancelled) return;
                    completedTasks += 1;
                    setProgress(
                        Math.round((completedTasks / startupTasks.length) * 100)
                    );
                    setStatus(`${label} hoàn tất`);
                };

                await Promise.all(
                    startupTasks.map(async ({ label, run }) => {
                        await run(startupManager);
                        updateProgress(label);
                    })
                );

                if (!cancelled) {
                    setStatus("Đã sẵn sàng");
                    setScreen("menu");
                }
            } catch (startupError) {
                if (!cancelled) {
                    setError(
                        startupError instanceof Error
                            ? startupError.message
                            : "Không thể hoàn tất khởi tạo ứng dụng."
                    );
                }
            }
        }

        startApplication();

        return () => {
            cancelled = true;
        };
    }, [attempt]);

    if (screen === "menu") return <MenuScreen />;

    return (
        <SplashScreen
            error={error}
            onRetry={() => setAttempt((currentAttempt) => currentAttempt + 1)}
            progress={progress}
            status={status}
        />
    );
}

export default App;
