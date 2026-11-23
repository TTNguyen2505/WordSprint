import { useId, useState } from "react";
import Button from "./components/Button";
import Modal from "./components/Modal";
import ModalTitle from "./components/ModalTitle";
import CreateUserModal from "./screens/CreateUserModal";
import LeaderboardModal from "./screens/LeaderboardModal";
import MenuScreen from "./screens/MenuScreen";
import PlayScreen from "./screens/PlayScreen";
import SelectProfileModal from "./screens/SelectProfileModal";
import StageResultModal from "./screens/StageResultModal";
import sampleStages from "./sampleStages";

const initialProfiles = [
    { id: 1, name: "TTNguyen2505", score: 124850, stages: sampleStages },
    ...Array.from({ length: 6 }, (_, index) => ({
        id: index + 2,
        name: `nguyen_0${index + 2}`,
        score: 124850,
        stages: sampleStages
    }))
];

function Application() {
    const [profiles, setProfiles] = useState(initialProfiles);
    const [activeId, setActiveId] = useState(initialProfiles[0].id);
    const [modal, setModal] = useState(null);
    const [selectedStage, setSelectedStage] = useState(null);
    const [stageRun, setStageRun] = useState(0);
    const [stageResult, setStageResult] = useState(null);
    const unavailableTitleId = useId();
    const activeProfile = profiles.find((profile) => profile.id === activeId);

    const createProfile = (name) => {
        const stages = sampleStages.map((stage, index) => ({
            ...stage,
            status: index === 0 ? "available" : "locked",
            score: 0
        }));
        const id = Math.max(0, ...profiles.map((profile) => profile.id)) + 1;
        setProfiles((current) => [...current, { id, name, score: 0, stages }]);
        setModal("profiles");
    };

    const deleteProfile = (profile) => {
        setProfiles((current) => current.filter(({ id }) => id !== profile.id));
    };

    const unavailable = (feature) => setModal({ unavailable: feature });

    const startStage = (stage) => {
        setSelectedStage(stage);
        setStageRun((current) => current + 1);
        setStageResult(null);
        setModal(null);
    };

    if (selectedStage) {
        const stageIndex = activeProfile.stages.findIndex(
            (stage) => stage.stageId === selectedStage.stageId
        );
        const nextStage = activeProfile.stages[stageIndex + 1];

        return (
            <>
                <div inert={Boolean(stageResult)}>
                    <PlayScreen
                        key={stageRun}
                        username={activeProfile.name}
                        topic={selectedStage.topic}
                        level={selectedStage.level}
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
                        onQuit={() => {
                            setSelectedStage(null);
                            setStageResult(null);
                        }}
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
                onSettings={() => unavailable("Settings")}
                onExit={() => unavailable("Exit game")}
                onStageAction={startStage}
            />
            {modal === "leaderboard" && (
                <LeaderboardModal onClose={() => setModal(null)} />
            )}
            {modal === "profiles" && (
                <SelectProfileModal
                    profiles={profiles.map((profile) => ({
                        ...profile,
                        progress: `${profile.stages.filter((stage) => stage.status === "solved").length}/${profile.stages.length}`
                    }))}
                    activeId={activeId}
                    onClose={() => setModal(null)}
                    onAdd={() => setModal("create")}
                    onSelectProfile={(profile) => setActiveId(profile.id)}
                    onDeleteProfile={deleteProfile}
                />
            )}
            {modal === "create" && (
                <CreateUserModal
                    onClose={() => setModal("profiles")}
                    onCreate={createProfile}
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
