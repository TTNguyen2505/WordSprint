/**
 * Format a duration in seconds as `m:ss`, returning `fallback` when missing.
 */
export function formatTime(seconds, fallback = "-") {
    if (seconds == null || !Number.isFinite(seconds)) return fallback;
    const totalSeconds = Math.max(0, Math.floor(seconds));
    const minutes = Math.floor(totalSeconds / 60);
    return `${minutes}:${String(totalSeconds % 60).padStart(2, "0")}`;
}
