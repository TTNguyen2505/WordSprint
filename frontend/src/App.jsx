import { useEffect, useState } from "react";
import MenuScreen from "@/components/MenuScreen.jsx";
import SplashScreen from "@/components/SplashScreen.jsx";
import { initializeApplication } from "./startup.js";

function App() {
    const [startup, setStartup] = useState({
        complete: false,
        databaseAvailable: false,
        databaseMessage: "",
        error: "",
        progress: 0,
        retry: 0,
        status: "Đang chuẩn bị WordSprint..."
    });

    useEffect(() => {
        let isMounted = true;

        initializeApplication((state) => {
            if (isMounted) {
                setStartup((current) => ({ ...current, ...state }));
            }
        })
            .then((database) => {
                if (isMounted) {
                    setStartup((current) => ({
                        ...current,
                        complete: true,
                        databaseAvailable: database.available,
                        databaseMessage: database.message || ""
                    }));
                }
            })
            .catch((error) => {
                if (isMounted) {
                    setStartup((current) => ({
                        ...current,
                        error:
                            error.message || "Không thể khởi động WordSprint."
                    }));
                }
            });

        return () => {
            isMounted = false;
        };
    }, [startup.retry]);

    if (startup.complete) {
        return (
            <MenuScreen
                databaseAvailable={startup.databaseAvailable}
                databaseMessage={startup.databaseMessage}
            />
        );
    }

    return (
        <SplashScreen
            error={startup.error}
            onRetry={() =>
                setStartup({
                    complete: false,
                    databaseAvailable: false,
                    databaseMessage: "",
                    error: "",
                    progress: 0,
                    retry: startup.retry + 1,
                    status: "Đang chuẩn bị WordSprint..."
                })
            }
            progress={startup.progress}
            status={startup.status}
        />
    );
}

export default App;
