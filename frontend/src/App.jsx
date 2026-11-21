import React, { useState, useEffect, useCallback } from "react";
import Modal from "./components/Modal.jsx";
import ModalTitle from "./components/ModalTitle.jsx";
import Slider from "./components/Slider.jsx";
import Button from "./components/Button.jsx";
import CloseIcon from "./assets/icons/close.svg?react";
import SaveIcon from "./assets/icons/save.svg?react";

// Default initial config as specified in the Figma design and prompt
const DEFAULT_AUDIO_SETTINGS = {
  bgm: { enabled: true, volume: 75 },
  sfx: { enabled: false, volume: 75 },
};

function App() {
  const [isOpen, setIsOpen] = useState(true);
  const [settings, setSettings] = useState(DEFAULT_AUDIO_SETTINGS);
  const [savedSettings, setSavedSettings] = useState(DEFAULT_AUDIO_SETTINGS);

  /**
   * Flexible configuration receiver function.
   * Can be invoked from external code/scripts via window.renderGameSettings(config).
   */
  const renderGameSettings = useCallback((newConfig) => {
    if (!newConfig) return;
    setSettings((prev) => {
      const updated = {
        bgm: {
          enabled: newConfig.bgm?.enabled ?? prev.bgm.enabled,
          volume: newConfig.bgm?.volume ?? prev.bgm.volume,
        },
        sfx: {
          enabled: newConfig.sfx?.enabled ?? prev.sfx.enabled,
          volume: newConfig.sfx?.volume ?? prev.sfx.volume,
        },
      };
      setSavedSettings(updated);
      return updated;
    });
  }, []);

  const handleToggle = useCallback((channel, nextEnabled) => {
    setSettings((prev) => ({
      ...prev,
      [channel]: {
        ...prev[channel],
        enabled: nextEnabled,
      },
    }));
  }, []);

  const handleVolumeChange = useCallback((channel, nextVolume) => {
    setSettings((prev) => ({
      ...prev,
      [channel]: {
        ...prev[channel],
        volume: nextVolume,
      },
    }));
  }, []);

  const handleSave = useCallback(() => {
    setSavedSettings(settings);
    if (typeof window.onSaveGameSettings === "function") {
      window.onSaveGameSettings(settings);
    }
    const event = new CustomEvent("game-settings-saved", { detail: settings });
    window.dispatchEvent(event);
  }, [settings]);

  const handleClose = useCallback(() => {
    // Revert uncommitted changes on close
    setSettings(savedSettings);
    setIsOpen(false);
    if (typeof window.onCloseGameSettings === "function") {
      window.onCloseGameSettings();
    }
  }, [savedSettings]);

  // Expose global methods for external modules and testing
  useEffect(() => {
    window.renderGameSettings = renderGameSettings;
    window.getGameSettings = () => settings;
    return () => {
      delete window.renderGameSettings;
      delete window.getGameSettings;
    };
  }, [renderGameSettings, settings]);

  return (
    <main className="flex min-h-screen w-full items-center justify-center bg-surface-canvas p-4 font-sans text-content-primary">
      {/* Re-open button if modal is closed */}
      {!isOpen && (
        <Button
          id="btn-open-modal"
          label="Open Game Settings"
          variant="primary"
          size="default"
          onClick={() => setIsOpen(true)}
          className="shadow-elevation-200"
        />
      )}

      {/* GAME SETTINGS Modal */}
      <Modal
        isOpen={isOpen}
        onClose={handleClose}
        maxWidth="max-w-[740px]"
        className="w-[740px] max-w-full h-[560px] max-h-[92vh] border-2 border-stroke-default p-8 sm:px-12 sm:py-8 shadow-elevation-200 flex flex-col justify-between items-center"
        overlayClassName="bg-black/50 backdrop-blur-[20px]"
        closeOnBackdropClick={true}
        closeOnEscape={true}
      >
        {/* Header with Title */}
        <header className="flex w-full items-center justify-center">
          <ModalTitle
            as="h1"
            className="text-center font-sans font-bold uppercase tracking-tight text-content-primary"
          >
            GAME SETTINGS
          </ModalTitle>
        </header>

        {/* Audio Controls Cards Container */}
        <section
          className="flex w-full flex-col items-center gap-6"
          aria-label="Audio Controls"
        >
          {/* Background Music Card */}
          <Slider
            id="bgm-slider-card"
            title={`Background Music (${settings.bgm.volume}%)`}
            enabled={settings.bgm.enabled}
            value={settings.bgm.volume}
            min={0}
            max={100}
            step={1}
            onToggle={(enabled) => handleToggle("bgm", enabled)}
            onChange={(volume) => handleVolumeChange("bgm", volume)}
            className="w-[640px] max-w-full"
          />

          {/* Sound Effect Card */}
          <Slider
            id="sfx-slider-card"
            title={`Sound Effect (${settings.sfx.volume}%)`}
            enabled={settings.sfx.enabled}
            value={settings.sfx.volume}
            min={0}
            max={100}
            step={1}
            onToggle={(enabled) => handleToggle("sfx", enabled)}
            onChange={(volume) => handleVolumeChange("sfx", volume)}
            className="w-[640px] max-w-full"
          />
        </section>

        {/* Footer with Action Buttons */}
        <footer className="flex w-full items-center justify-center gap-12">
          <Button
            id="btn-close-settings"
            label="Close"
            variant="secondary"
            size="default"
            leadingIcon={<CloseIcon className="size-3.5" />}
            showTrailingIcon={false}
            onClick={handleClose}
            className="w-[120px] h-[40px] border-2 border-stroke-default shadow-elevation-200 text-button-default"
          />
          <Button
            id="btn-save-settings"
            label="Save"
            variant="primary"
            size="default"
            leadingIcon={<SaveIcon className="size-4" />}
            showTrailingIcon={false}
            onClick={handleSave}
            className="w-[120px] h-[40px] border-2 border-stroke-default shadow-elevation-200 text-button-default bg-surface-primary-cta"
          />
        </footer>
      </Modal>
    </main>
  );
}

export default App;
