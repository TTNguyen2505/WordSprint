import { useId, useState } from "react";
import ExitIcon from "../assets/icons/exit.svg?react";
import PlayIcon from "../assets/icons/play.svg?react";
import ReplayIcon from "../assets/icons/replay.svg?react";
import AudioSettings from "../components/composite/AudioSettings";
import Button from "../components/primitives/Button";
import InfoPill from "../components/primitives/InfoPill";
import Modal from "../components/primitives/Modal";
import ModalTitle from "../components/primitives/ModalTitle";

function PauseModal({
    isOpen = true,
    solved = 3,
    total = 8,
    timeLeft = "8:00",
    score = 450,
    initialMusicEnabled = true,
    initialMusicVolume = 100,
    initialSoundEnabled = true,
    initialSoundVolume = 100,
    onMusicToggle,
    onMusicVolumeChange,
    onSoundToggle,
    onSoundVolumeChange,
    onAudioSettingsChange,
    onSaveSettings,
    onResume,
    onReplay,
    onQuit
}) {
    const titleId = useId();
    const [settings, setSettings] = useState(() => ({
        musicEnabled: initialMusicEnabled,
        musicVolume: initialMusicVolume,
        soundEnabled: initialSoundEnabled,
        soundVolume: initialSoundVolume
    }));

    const settingCallbacks = {
        musicEnabled: onMusicToggle,
        musicVolume: onMusicVolumeChange,
        soundEnabled: onSoundToggle,
        soundVolume: onSoundVolumeChange
    };

    const updateSetting = (key, value) => {
        settingCallbacks[key]?.(value);
        setSettings((current) => {
            const nextSettings = { ...current, [key]: value };
            onAudioSettingsChange?.(nextSettings);
            return nextSettings;
        });
    };

    const withSavedSettings = (action) => () => {
        Promise.resolve(onSaveSettings?.(settings)).catch(() => {});
        action?.();
    };

    const handleResume = withSavedSettings(onResume);

    return (
        <Modal
            isOpen={isOpen}
            onClose={handleResume}
            aria-labelledby={titleId}
            header={<ModalTitle id={titleId}>GAME PAUSED</ModalTitle>}
            body={
                <div className="flex w-[640px] flex-col gap-xl">
                    <div className="flex items-center justify-between gap-xs">
                        <InfoPill
                            label="Quiz Solved"
                            value={`${solved}/${total}`}
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                        />
                        <InfoPill
                            label="Time left"
                            value={timeLeft}
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                        />
                        <InfoPill
                            label="Total Score"
                            value={score}
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                        />
                    </div>
                    <AudioSettings
                        {...settings}
                        onMusicToggle={(value) =>
                            updateSetting("musicEnabled", value)
                        }
                        onMusicVolumeChange={(value) =>
                            updateSetting("musicVolume", value)
                        }
                        onSoundToggle={(value) =>
                            updateSetting("soundEnabled", value)
                        }
                        onSoundVolumeChange={(value) =>
                            updateSetting("soundVolume", value)
                        }
                    />
                </div>
            }
            footer={
                <div className="flex w-[640px] flex-col gap-md">
                    <Button
                        label="RESUME GAME"
                        size="large"
                        className="w-full"
                        leadingIcon={<PlayIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={handleResume}
                    />
                    <div className="flex gap-lg">
                        <Button
                            label="REPLAY"
                            variant="secondary"
                            size="large"
                            className="flex-1"
                            leadingIcon={<ReplayIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={withSavedSettings(onReplay)}
                        />
                        <Button
                            label="QUIT TO MENU"
                            variant="destructive"
                            size="large"
                            className="flex-1"
                            leadingIcon={<ExitIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={withSavedSettings(onQuit)}
                        />
                    </div>
                </div>
            }
        />
    );
}

export default PauseModal;
