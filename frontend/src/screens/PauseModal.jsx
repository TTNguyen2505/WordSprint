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
    initialMusicVolume = 75,
    initialSoundEnabled = false,
    initialSoundVolume = 75,
    onMusicToggle,
    onMusicVolumeChange,
    onSoundToggle,
    onSoundVolumeChange,
    onResume,
    onReplay,
    onQuit
}) {
    const titleId = useId();
    const [musicEnabled, setMusicEnabled] = useState(initialMusicEnabled);
    const [musicVolume, setMusicVolume] = useState(initialMusicVolume);
    const [soundEnabled, setSoundEnabled] = useState(initialSoundEnabled);
    const [soundVolume, setSoundVolume] = useState(initialSoundVolume);

    return (
        <Modal
            isOpen={isOpen}
            onClose={onResume}
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
                        }}
                        onMusicVolumeChange={(value) => {
                            setMusicVolume(value);
                            onMusicVolumeChange?.(value);
                        }}
                        onSoundToggle={(enabled) => {
                            setSoundEnabled(enabled);
                            onSoundToggle?.(enabled);
                        }}
                        onSoundVolumeChange={(value) => {
                            setSoundVolume(value);
                            onSoundVolumeChange?.(value);
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
                        onClick={onResume}
                    />
                    <div className="flex gap-lg">
                        <Button
                            label="REPLAY"
                            variant="secondary"
                            size="large"
                            className="flex-1"
                            leadingIcon={<ReplayIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={onReplay}
                        />
                        <Button
                            label="QUIT TO MENU"
                            variant="destructive"
                            size="large"
                            className="flex-1"
                            leadingIcon={<ExitIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={onQuit}
                        />
                    </div>
                </div>
            }
        />
    );
}

export default PauseModal;
