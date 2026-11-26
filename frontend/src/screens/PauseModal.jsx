import { useId, useState } from "react";
import ExitIcon from "../assets/icons/exit.svg?react";
import PlayIcon from "../assets/icons/play.svg?react";
import ReplayIcon from "../assets/icons/replay.svg?react";
import AudioSettings from "../components/AudioSettings";
import Button from "../components/Button";
import InfoPill from "../components/InfoPill";
import Modal from "../components/Modal";
import ModalTitle from "../components/ModalTitle";

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
    const [musicEnabled, setMusicEnabled] = useState(initialMusicEnabled);
    const [musicVolume, setMusicVolume] = useState(initialMusicVolume);
    const [soundEnabled, setSoundEnabled] = useState(initialSoundEnabled);
    const [soundVolume, setSoundVolume] = useState(initialSoundVolume);
    const previewSettings = (nextSettings) => {
        onAudioSettingsChange?.(nextSettings);
    };
    const saveSettings = () => {
        Promise.resolve(
            onSaveSettings?.({
                musicEnabled,
                musicVolume,
                soundEnabled,
                soundVolume
            })
        ).catch(() => {});
    };
    const handleResume = () => {
        saveSettings();
        onResume?.();
    };
    const handleReplay = () => {
        saveSettings();
        onReplay?.();
    };
    const handleQuit = () => {
        saveSettings();
        onQuit?.();
    };

    return (
        <Modal
            isOpen={isOpen}
            onClose={handleResume}
            aria-labelledby={titleId}
            header={
                <ModalTitle
                    id={titleId}
                    className="w-full text-center text-[length:var(--primitive-font-size-36)] leading-[var(--primitive-line-height-40)]"
                >
                    GAME PAUSED
                </ModalTitle>
            }
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
                        musicEnabled={musicEnabled}
                        musicVolume={musicVolume}
                        soundEnabled={soundEnabled}
                        soundVolume={soundVolume}
                        onMusicToggle={(enabled) => {
                            setMusicEnabled(enabled);
                            onMusicToggle?.(enabled);
                            previewSettings({
                                musicEnabled: enabled,
                                musicVolume,
                                soundEnabled,
                                soundVolume
                            });
                        }}
                        onMusicVolumeChange={(value) => {
                            setMusicVolume(value);
                            onMusicVolumeChange?.(value);
                            previewSettings({
                                musicEnabled,
                                musicVolume: value,
                                soundEnabled,
                                soundVolume
                            });
                        }}
                        onSoundToggle={(enabled) => {
                            setSoundEnabled(enabled);
                            onSoundToggle?.(enabled);
                            previewSettings({
                                musicEnabled,
                                musicVolume,
                                soundEnabled: enabled,
                                soundVolume
                            });
                        }}
                        onSoundVolumeChange={(value) => {
                            setSoundVolume(value);
                            onSoundVolumeChange?.(value);
                            previewSettings({
                                musicEnabled,
                                musicVolume,
                                soundEnabled,
                                soundVolume: value
                            });
                        }}
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
                            onClick={handleReplay}
                        />
                        <Button
                            label="QUIT TO MENU"
                            variant="destructive"
                            size="large"
                            className="flex-1"
                            leadingIcon={<ExitIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={handleQuit}
                        />
                    </div>
                </div>
            }
        />
    );
}

export default PauseModal;
