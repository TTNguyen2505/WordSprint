import { useEffect, useId, useRef, useState } from "react";
import Button from "./components/Button";
import Modal from "./components/Modal";
import ModalTitle from "./components/ModalTitle";
import CreateUserModal from "./screens/CreateUserModal";
import GameSettingsModal from "./screens/GameSettingsModal";
import LeaderboardModal from "./screens/LeaderboardModal";
import MenuScreen from "./screens/MenuScreen";
import PlayScreen from "./screens/PlayScreen";
import SelectProfileModal from "./screens/SelectProfileModal";
import StageResultModal from "./screens/StageResultModal";
import { startStage as loadStageQuestions } from "./qtBridge.js";
import { useAppSessionStore } from "./viewModels/appSessionStore";
import { useMenuViewModel } from "./viewModels/menuViewModel";

function formatTimeLimit(seconds) {
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = seconds % 60;
    return `${String(minutes).padStart(2, "0")}:${String(remainingSeconds).padStart(2, "0")}`;
}

function Application() {
    const [modal, setModal] = useState(null);
    const [selectedStage, setSelectedStage] = useState(null);
    const [stageRun, setStageRun] = useState(0);
    const [stageResult, setStageResult] = useState(null);
    const [stagePayload, setStagePayload] = useState(null);
    const [stageLoadError, setStageLoadError] = useState(null);
    const stageLoadRequest = useRef(0);
    const unavailableTitleId = useId();
    const activeProfile = useMenuViewModel();
    const status = useAppSessionStore((state) => state.status);
    const error = useAppSessionStore((state) => state.error);
    const loadProfiles = useAppSessionStore((state) => state.loadProfiles);
    const createProfile = useAppSessionStore((state) => state.createProfile);
    const updateAudio = useAppSessionStore((state) => state.updateAudio);

    useEffect(() => {
        loadProfiles().catch(() => {});
    }, [loadProfiles]);

    const unavailable = (feature) => setModal({ unavailable: feature });

    const startStage = async (stage) => {
        const requestId = ++stageLoadRequest.current;
        setSelectedStage(stage);
        setStageRun((current) => current + 1);
        setStageResult(null);
        setStagePayload(null);
        setStageLoadError(null);
        setModal(null);

        try {
            const response = await loadStageQuestions({ stageId: stage.id });
            if (response.status !== "ok") {
                throw new Error(
                    response.message || "Could not load stage questions."
                );
            }
            if (requestId === stageLoadRequest.current)
                setStagePayload(response);
        } catch (loadError) {
            if (requestId === stageLoadRequest.current) {
                setStageLoadError(
                    loadError.message || "Could not load stage questions."
                );
            }
        }
    };

    const quitStage = () => {
        ++stageLoadRequest.current;
        setSelectedStage(null);
        setStageResult(null);
        setStagePayload(null);
        setStageLoadError(null);
        setModal(null);
    };

    if (status !== "ready") {
        return (
            <main className="flex h-[900px] w-[1200px] flex-col items-center justify-center gap-md bg-surface-canvas">
                {status === "error" ? (
                    <>
                        <p role="alert">{error}</p>
                        <Button
                            label="Retry"
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                            onClick={() => loadProfiles().catch(() => {})}
                        />
                    </>
                ) : (
                    <p role="status">Loading profiles...</p>
                )}
            </main>
        );
    }

    if (!activeProfile) {
        return (
            <main className="flex h-[900px] w-[1200px] flex-col items-center justify-center gap-md bg-surface-canvas">
                <p>No profiles yet. Create one to begin.</p>
                <Button
                    label="Create profile"
                    showLeadingIcon={false}
                    showTrailingIcon={false}
                    onClick={() => setModal("create")}
                />
                {modal === "create" && (
                    <CreateUserModal
                        onClose={() => setModal(null)}
                        onCreate={async (name) => {
                            await createProfile(name);
                            setModal(null);
                        }}
                    />
                )}
            </main>
        );
    }

    if (selectedStage) {
        const nextStage = activeProfile.stages.find(
            (stage) => stage.id === selectedStage.previewNextStageId
        );

        return (
            <>
                <div inert={Boolean(stageResult)}>
                    <PlayScreen
                        key={stageRun}
                        username={activeProfile.name}
                        topic={selectedStage.topic}
                        level={stagePayload?.cefrLevel ?? selectedStage.level}
                        time={
                            stagePayload
                                ? formatTimeLimit(stagePayload.timeLimit)
                                : "10:00"
                        }
                        questions={stagePayload?.words ?? []}
                        loadingQuestions={!stagePayload && !stageLoadError}
                        loadError={stageLoadError}
                        onReplay={() => startStage(selectedStage)}
                        onQuit={quitStage}
                        onPreviewResult={(result) =>
                            setStageResult(
                                result === "timeUp"
                                    ? "timeUp"
                                    : nextStage
                                      ? "stageCleared"
                                      : "campaignCleared"
                            )
                        }
                    />
                </div>
                {stageResult && modal !== "leaderboard" && (
                    <StageResultModal
                        result={stageResult}
                        stageTitle={`${selectedStage.stageId}: ${selectedStage.topic}`}
                        nextStageTitle={
                            nextStage
                                ? `${nextStage.stageId}: ${nextStage.topic}`
                                : undefined
                        }
                        nextStageLevel={nextStage?.level}
                        onNextStage={() => startStage(nextStage)}
                        onReplay={() => startStage(selectedStage)}
                        onQuit={quitStage}
                        onLeaderboard={() => setModal("leaderboard")}
                    />
                )}
                {modal === "leaderboard" && (
                    <LeaderboardModal onClose={() => setModal(null)} />
                )}
            </>
        );
    }

    return (
        <>
            <MenuScreen
                username={activeProfile.name}
                score={activeProfile.score}
                stages={activeProfile.stages}
                onSwitchProfile={() => setModal("profiles")}
                onLeaderboard={() => setModal("leaderboard")}
                onPlay={() => unavailable("Gameplay")}
                onHelp={() => unavailable("Help")}
                onSettings={() => setModal("settings")}
                onExit={() => unavailable("Exit game")}
                onStageAction={startStage}
            />
            {modal === "leaderboard" && (
                <LeaderboardModal onClose={() => setModal(null)} />
            )}
            {modal === "settings" && (
                <GameSettingsModal
                    initialMusicEnabled={activeProfile.musicEnabled}
                    initialMusicVolume={activeProfile.musicVolume}
                    initialSoundEnabled={activeProfile.soundEnabled}
                    initialSoundVolume={activeProfile.soundVolume}
                    onSave={updateAudio}
                    onClose={() => setModal(null)}
                />
            )}
            {modal === "profiles" && (
                <SelectProfileModal
                    onClose={() => setModal(null)}
                    onAdd={() => setModal("create")}
                />
            )}
            {modal === "create" && (
                <CreateUserModal
                    onClose={() => setModal("profiles")}
                    onCreate={async (name) => {
                        await createProfile(name);
                        setModal("profiles");
                    }}
                />
            )}
            {typeof modal === "object" && modal !== null && (
                <Modal
                    onClose={() => setModal(null)}
                    aria-labelledby={unavailableTitleId}
                    maxWidth="max-w-[580px]"
                    header={
                        <ModalTitle
                            id={unavailableTitleId}
                            className="w-full text-center"
                        >
                            {modal.unavailable}
                        </ModalTitle>
                    }
                    body={
                        <p className="text-body-default text-center">
                            This screen is not available yet.
                        </p>
                    }
                    footer={
                        <Button
                            label="Close"
                            size="default"
                            variant="secondary"
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                            onClick={() => setModal(null)}
                        />
                    }
                />
            )}
        </>
    );
}

export default Application;
