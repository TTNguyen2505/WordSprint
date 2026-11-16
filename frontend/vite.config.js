import { fileURLToPath } from "node:url";
import tailwindcss from "@tailwindcss/vite";
import react from "@vitejs/plugin-react-swc";
import svgr from "vite-plugin-svgr";
import { defineConfig } from "vite";

export default defineConfig({
    resolve: {
        alias: {
            "@": fileURLToPath(new URL("./src", import.meta.url))
        }
    },
    plugins: [
        tailwindcss(),
        react(),
        svgr({ svgrOptions: { replaceAttrValues: { black: "currentColor" } } })
    ],
    base: "./",
    server: {
        port: 5173,
        strictPort: true
    }
});
