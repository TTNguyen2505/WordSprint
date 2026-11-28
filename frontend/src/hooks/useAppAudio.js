import { useCallback, useEffect, useRef } from "react";
import backgroundMusicUrl from "../assets/sounds/background-music.mp3";
import clickEffectUrl from "../assets/sounds/click-effect.mp3";
import hoverEffectUrl from "../assets/sounds/hover-effect.mp3";

const DEFAULT_SETTINGS = {
    musicEnabled: true,
    musicVolume: 100,
    soundEnabled: true,
    soundVolume: 100
};

export function useAppAudio(initialSettings = DEFAULT_SETTINGS) {
    const settingsRef = useRef({ ...DEFAULT_SETTINGS, ...initialSettings });
    const initialMusicEnabled =
        initialSettings?.musicEnabled ?? DEFAULT_SETTINGS.musicEnabled;
    const initialMusicVolume =
        initialSettings?.musicVolume ?? DEFAULT_SETTINGS.musicVolume;
    const initialSoundEnabled =
        initialSettings?.soundEnabled ?? DEFAULT_SETTINGS.soundEnabled;
    const initialSoundVolume =
        initialSettings?.soundVolume ?? DEFAULT_SETTINGS.soundVolume;
    const backgroundMusicRef = useRef(null);
    const hoverEffectRef = useRef(null);

    const playSoundEffect = useCallback((url) => {
        const settings = settingsRef.current;
        if (!settings.soundEnabled) return;

        const effect =
            url === hoverEffectUrl ? hoverEffectRef.current : new Audio(url);
        if (!effect) return;

        effect.volume = settings.soundVolume / 100;
        effect.currentTime = 0;
        effect.play().catch(() => {});
    }, []);

    const applySettings = useCallback((settings) => {
        settingsRef.current = { ...settingsRef.current, ...settings };
        const current = settingsRef.current;
        const music = backgroundMusicRef.current;

        if (music) {
            music.volume = current.musicVolume / 100;
            if (current.musicEnabled) {
                music.play().catch(() => {});
            } else {
                music.pause();
            }
        }
    }, []);

    useEffect(() => {
        const music = new Audio(backgroundMusicUrl);
        music.loop = true;
        music.preload = "auto";
        backgroundMusicRef.current = music;
        hoverEffectRef.current = new Audio(hoverEffectUrl);
        applySettings(settingsRef.current);

        const unlockAudio = () => {
            applySettings(settingsRef.current);
        };
        const handleClick = (event) => {
            if (
                event.target instanceof Element &&
                event.target.closest("button, input, [role='button']")
            ) {
                playSoundEffect(clickEffectUrl);
            }
        };
        const handlePointerOver = (event) => {
            if (!(event.target instanceof Element)) return;

            const control = event.target.closest(
                "button, input, [role='button']"
            );
            if (!control) return;

            const previousControl =
                event.relatedTarget instanceof Element
                    ? event.relatedTarget.closest(
                          "button, input, [role='button']"
                      )
                    : null;

            if (previousControl !== control) {
                playSoundEffect(hoverEffectUrl);
            }
        };

        document.addEventListener("pointerdown", unlockAudio, { once: true });
        document.addEventListener("keydown", unlockAudio, { once: true });
        document.addEventListener("click", handleClick);
        document.addEventListener("pointerover", handlePointerOver);

        return () => {
            document.removeEventListener("pointerdown", unlockAudio);
            document.removeEventListener("keydown", unlockAudio);
            document.removeEventListener("click", handleClick);
            document.removeEventListener("pointerover", handlePointerOver);
            music.pause();
            backgroundMusicRef.current = null;
            hoverEffectRef.current = null;
        };
    }, [applySettings, playSoundEffect]);

    useEffect(() => {
        applySettings({
            musicEnabled: initialMusicEnabled,
            musicVolume: initialMusicVolume,
            soundEnabled: initialSoundEnabled,
            soundVolume: initialSoundVolume
        });
    }, [
        applySettings,
        initialMusicEnabled,
        initialMusicVolume,
        initialSoundEnabled,
        initialSoundVolume
    ]);

    return {
        applySettings,
        playClickEffect: () => playSoundEffect(clickEffectUrl),
        playHoverEffect: () => playSoundEffect(hoverEffectUrl)
    };
}
