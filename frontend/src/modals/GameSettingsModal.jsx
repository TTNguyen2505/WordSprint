import { useId, useState } from "react";
import CloseIcon from "../assets/icons/close.svg?react";
import SaveIcon from "../assets/icons/save.svg?react";
import AudioSettings from "../components/composite/AudioSettings";
import Button from "../components/primitives/Button";
import Modal from "../components/primitives/Modal";
import ModalTitle from "../components/primitives/ModalTitle";

function GameSettingsModal({
    isOpen = true,
    initialMusicEnabled = true,
    initialMusicVolume = 100,
    initialSoundEnabled = true,
    initialSoundVolume = 100,
    onClose,
    onSave,
    onPreviewSettings
}) {
    const titleId = useId();
    const [savedSettings, setSavedSettings] = useState(() => ({
        musicEnabled: initialMusicEnabled,
        musicVolume: initialMusicVolume,
        soundEnabled: initialSoundEnabled,
        soundVolume: initialSoundVolume
    }));
    const [settings, setSettings] = useState(savedSettings);
    const [error, setError] = useState("");
    const [isSaving, setIsSaving] = useState(false);

    const updateSetting = (key, value) => {
        setSettings((current) => {
            const nextSettings = { ...current, [key]: value };
            onPreviewSettings?.(nextSettings);
            return nextSettings;
        });
    };

    const handleClose = () => {
        setSettings(savedSettings);
        onPreviewSettings?.(savedSettings);
        onClose?.();
    };

    const handleSave = async () => {
        setError("");
        setIsSaving(true);
        try {
            await onSave?.(settings);
            setSavedSettings(settings);
            onClose?.();
        } catch (saveError) {
            setError(saveError.message);
        } finally {
            setIsSaving(false);
        }
    };

    return (
        <Modal
            isOpen={isOpen}
            onClose={handleClose}
            aria-labelledby={titleId}
            header={<ModalTitle id={titleId}>GAME SETTINGS</ModalTitle>}
            body={
                <div>
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
                    {error && <p role="alert">{error}</p>}
                </div>
            }
            footer={
                <div className="flex gap-2xl">
                    <Button
                        label="Close"
                        size="default"
                        variant="secondary"
                        leadingIcon={<CloseIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={handleClose}
                    />
                    <Button
                        label="Save"
                        size="default"
                        leadingIcon={<SaveIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={handleSave}
                        disabled={isSaving}
                    />
                </div>
            }
        />
    );
}

export default GameSettingsModal;
