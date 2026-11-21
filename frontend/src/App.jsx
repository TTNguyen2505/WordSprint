import React, { useState, useEffect, useCallback } from "react";
import Modal from "./components/Modal.jsx";
import ModalTitle from "./components/ModalTitle.jsx";
import Slider from "./components/Slider.jsx";
import InfoPill from "./components/InfoPill.jsx";
import Button from "./components/Button.jsx";
import PlayIcon from "./assets/icons/play.svg?react";
import ReplayIcon from "./assets/icons/replay.svg?react";
import ExitIcon from "./assets/icons/exit.svg?react";

// Default initial simulation state (Figma values: Quiz Solved 3/8, Time left 8:00, Total Score 450)
const DEFAULT_GAME_STATE = {
  solved: 3,
  totalQuiz: 8,
  timeLeft: 480, // 480s = 8:00
  score: 450,
};

const DEFAULT_AUDIO_SETTINGS = {
  bgm: { enabled: true, volume: 75 },
  sfx: { enabled: false, volume: 75 },
};

function formatTimeLeft(secondsOrStr) {
  if (typeof secondsOrStr === "string") return secondsOrStr;
  if (typeof secondsOrStr === "number") {
    const mins = Math.floor(secondsOrStr / 60);
    const secs = secondsOrStr % 60;
    return `${mins}:${secs < 10 ? "0" : ""}${secs}`;
  }
  return "0:00";
}

function formatQuizSolved(solved, total) {
  if (typeof solved === "string" && solved.includes("/")) return solved;
  return `${solved ?? 0}/${total ?? 0}`;
}

function App() {
  const [isOpen, setIsOpen] = useState(true);
  const [gameState, setGameState] = useState(DEFAULT_GAME_STATE);
  const [audioSettings, setAudioSettings] = useState(DEFAULT_AUDIO_SETTINGS);

  /**
   * Flexible state receiver function.
   * Can be invoked from game loops or testing scripts via window.updatePauseMenu(data)
   * or window.renderPauseMenu(data).
   */
  const updatePauseMenu = useCallback((data) => {
    if (!data) return;
    if (
      data.solved !== undefined ||
      data.totalQuiz !== undefined ||
      data.timeLeft !== undefined ||
      data.score !== undefined
    ) {
      setGameState((prev) => ({
        solved: data.solved ?? prev.solved,
        totalQuiz: data.totalQuiz ?? prev.totalQuiz,
        timeLeft: data.timeLeft ?? prev.timeLeft,
        score: data.score ?? prev.score,
      }));
    }
    if (data.audioSettings) {
      setAudioSettings((prev) => ({
        bgm: {
          enabled: data.audioSettings.bgm?.enabled ?? prev.bgm.enabled,
          volume: data.audioSettings.bgm?.volume ?? prev.bgm.volume,
        },
        sfx: {
          enabled: data.audioSettings.sfx?.enabled ?? prev.sfx.enabled,
          volume: data.audioSettings.sfx?.volume ?? prev.sfx.volume,
        },
      }));
    }
  }, []);

  const handleAudioToggle = useCallback((channel, nextEnabled) => {
    setAudioSettings((prev) => {
      const updated = {
        ...prev,
        [channel]: { ...prev[channel], enabled: nextEnabled },
      };
      if (typeof window.onAudioSettingsChange === "function") {
        window.onAudioSettingsChange(updated);
      }
      window.dispatchEvent(
        new CustomEvent("audio-settings-changed", { detail: updated })
      );
      return updated;
    });
  }, []);

  const handleAudioVolumeChange = useCallback((channel, nextVolume) => {
    setAudioSettings((prev) => {
      const updated = {
        ...prev,
        [channel]: { ...prev[channel], volume: nextVolume },
      };
      if (typeof window.onAudioSettingsChange === "function") {
        window.onAudioSettingsChange(updated);
      }
      window.dispatchEvent(
        new CustomEvent("audio-settings-changed", { detail: updated })
      );
      return updated;
    });
  }, []);

  // Action button handlers
  const handleResume = useCallback(() => {
    setIsOpen(false);
    if (typeof window.onResumeGame === "function") {
      window.onResumeGame();
    }
    window.dispatchEvent(
      new CustomEvent("game-resumed", { detail: { gameState, audioSettings } })
    );
  }, [gameState, audioSettings]);

  const handleReplay = useCallback(() => {
    if (typeof window.onReplayGame === "function") {
      window.onReplayGame();
    }
    window.dispatchEvent(
      new CustomEvent("game-replay", { detail: { gameState, audioSettings } })
    );
  }, [gameState, audioSettings]);

  const handleQuit = useCallback(() => {
    if (typeof window.onQuitToMenu === "function") {
      window.onQuitToMenu();
    }
    window.dispatchEvent(
      new CustomEvent("game-quit", { detail: { gameState, audioSettings } })
    );
  }, [gameState, audioSettings]);

  // Expose global methods for external modules and testing
  useEffect(() => {
    window.updatePauseMenu = updatePauseMenu;
    window.renderPauseMenu = updatePauseMenu;
    window.getPauseMenuState = () => ({ gameState, audioSettings, isOpen });
    return () => {
      delete window.updatePauseMenu;
      delete window.renderPauseMenu;
      delete window.getPauseMenuState;
    };
  }, [updatePauseMenu, gameState, audioSettings, isOpen]);

  return (
    <main className="flex min-h-screen w-full items-center justify-center bg-surface-canvas p-4 font-sans text-content-primary">
      {/* Re-open button if modal is closed */}
      {!isOpen && (
        <Button
          id="btn-open-modal"
          label="Open Pause Menu"
          variant="primary"
          size="default"
          onClick={() => setIsOpen(true)}
          className="shadow-elevation-200"
        />
      )}

      {/* GAME PAUSED Modal */}
      <Modal
        isOpen={isOpen}
        onClose={handleResume}
        maxWidth="max-w-[740px]"
        className="w-[740px] max-w-full h-[710px] min-h-[710px] max-h-[95vh] border-2 border-stroke-default p-[32px] sm:p-[48px] shadow-elevation-200 flex flex-col justify-between items-center bg-surface-panel"
        overlayClassName="bg-black/50 backdrop-blur-[20px]"
        closeOnBackdropClick={true}
        closeOnEscape={true}
      >
        {/* Header with Title */}
        <header className="flex w-full items-center justify-center pt-[8px]">
          <ModalTitle
            as="h1"
            className="text-center font-sans font-bold uppercase tracking-tight text-content-primary"
          >
            GAME PAUSED
          </ModalTitle>
        </header>

        {/* 1. Stage States: 3 Info Pill stats boxes */}
        <section
          className="flex w-full max-w-[640px] items-center justify-between gap-[16px]"
          aria-label="Game Statistics"
        >
          <InfoPill
            id="pill-quiz-solved"
            label="Quiz Solved"
            value={formatQuizSolved(gameState.solved, gameState.totalQuiz)}
            showLeadingIcon={false}
            showTrailingIcon={false}
            className="flex-1 justify-center shadow-elevation-200"
          />
          <InfoPill
            id="pill-time-left"
            label="Time left"
            value={formatTimeLeft(gameState.timeLeft)}
            showLeadingIcon={false}
            showTrailingIcon={false}
            className="flex-1 justify-center shadow-elevation-200"
          />
          <InfoPill
            id="pill-total-score"
            label="Total Score"
            value={gameState.score}
            showLeadingIcon={false}
            showTrailingIcon={false}
            className="flex-1 justify-center shadow-elevation-200"
          />
        </section>

        {/* 2. Audio Controls: Background Music and Sound Effect cards */}
        <section
          className="flex w-full max-w-[640px] flex-col items-center gap-[24px]"
          aria-label="Audio Settings"
        >
          {/* Background Music Card */}
          <Slider
            id="bgm-slider-card"
            title={`Background Music (${audioSettings.bgm.volume}%)`}
            enabled={audioSettings.bgm.enabled}
            value={audioSettings.bgm.volume}
            min={0}
            max={100}
            step={1}
            onToggle={(enabled) => handleAudioToggle("bgm", enabled)}
            onChange={(volume) => handleAudioVolumeChange("bgm", volume)}
            className="w-[640px] max-w-full"
          />

          {/* Sound Effect Card */}
          <Slider
            id="sfx-slider-card"
            title={`Sound Effect (${audioSettings.sfx.volume}%)`}
            enabled={audioSettings.sfx.enabled}
            value={audioSettings.sfx.volume}
            min={0}
            max={100}
            step={1}
            onToggle={(enabled) => handleAudioToggle("sfx", enabled)}
            onChange={(volume) => handleAudioVolumeChange("sfx", volume)}
            className="w-[640px] max-w-full"
          />
        </section>

        {/* 3. Action Navigation Buttons */}
        <footer
          className="flex w-full max-w-[640px] flex-col gap-[16px] pb-[8px]"
          aria-label="Modal Actions"
        >
          {/* Main Action: RESUME GAME */}
          <Button
            id="btn-resume-game"
            label="RESUME GAME"
            variant="primary"
            size="large"
            leadingIcon={<PlayIcon className="size-[20px] [&_path]:fill-current" />}
            showTrailingIcon={false}
            onClick={handleResume}
            className="w-full h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-primary-cta"
          />

          {/* Secondary Actions: REPLAY and QUIT TO MENU */}
          <div className="flex w-full items-center justify-between gap-[24px]">
            <Button
              id="btn-replay-game"
              label="REPLAY"
              variant="secondary"
              size="large"
              leadingIcon={<ReplayIcon className="size-[20px] [&_path]:fill-current" />}
              showTrailingIcon={false}
              onClick={handleReplay}
              className="flex-1 h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold"
            />
            <Button
              id="btn-quit-menu"
              label="QUIT TO MENU"
              variant="destructive"
              size="large"
              leadingIcon={<ExitIcon className="size-[20px] [&_path]:fill-content-inverse" />}
              showTrailingIcon={false}
              onClick={handleQuit}
              className="flex-1 h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-danger text-content-inverse"
            />
          </div>
        </footer>
      </Modal>
    </main>
  );
}

export default App;
