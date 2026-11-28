import { useId } from "react";
import ArrowForwardIcon from "../assets/icons/arrow_forward_2.svg?react";
import ExitIcon from "../assets/icons/exit.svg?react";
import ReplayIcon from "../assets/icons/replay.svg?react";
import TrophyIcon from "../assets/icons/trophy.svg?react";
import Badge from "../components/Badge";
import Button from "../components/Button";
import InfoPill from "../components/InfoPill";
import Modal from "../components/Modal";
import ModalTitle from "../components/ModalTitle";
import ProgressSummaryCard from "../components/ProgressSummaryCard";

const mockResults = {
    stageCleared: {
        badge: "STAGE CLEARED",
        stageTitle: "Stage 1: School",
        solved: 8,
        playTime: "8:00",
        score: 450
    },
    campaignCleared: {
        badge: "ALL STAGES CLEARED",
        stageTitle: "Stage 5: School",
        solved: 8,
        playTime: "8:00",
        score: 450
    },
    timeUp: {
        badge: "TIME'S UP",
        stageTitle: "Stage 1: School",
        solved: 3,
        playTime: "10:00",
        score: 120
    },
    stageCompleted: {
        badge: "STAGE COMPLETED",
        stageTitle: "Stage 1: School",
        solved: 0,
        playTime: "0:00",
        score: 0
    },
    failed: {
        badge: "STAGE NOT CLEARED",
        stageTitle: "Stage 1: School",
        solved: 0,
        playTime: "0:00",
        score: 0
    }
};

function StageResultModal({
    isOpen = true,
    result = "stageCleared",
    stageTitle,
    solved,
    total = 8,
    playTime,
    score,
    rank,
    nextStageTitle,
    nextStageLevel = "A2",
    campaignScore = 5420,
    unlockRequirement = 4,
    passScore,
    passed = result !== "timeUp" &&
        result !== "failed" &&
        result !== "stageCompleted",
    onClose,
    onNextStage,
    onLeaderboard,
    onReplay,
    onQuit
}) {
    const titleId = useId();
    const mock = mockResults[result] ?? mockResults.stageCompleted;
    const isCampaignCleared = result === "campaignCleared";
    const canAdvance = passed && Boolean(onNextStage);
    const retryFirst = !passed;

    return (
        <Modal
            isOpen={isOpen}
            onClose={onClose}
            aria-labelledby={titleId}
            header={
                <div className="flex w-full flex-col items-center gap-lg">
                    <Badge
                        type={passed ? "Success" : "Danger"}
                        showIcon={false}
                    >
                        {mock.badge}
                    </Badge>
                    <ModalTitle
                        id={titleId}
                        className="w-full text-center text-[length:var(--primitive-font-size-36)] leading-[var(--primitive-line-height-40)] uppercase"
                    >
                        {stageTitle ?? mock.stageTitle}
                    </ModalTitle>
                </div>
            }
            body={
                <div className="flex w-[640px] flex-col items-center gap-xl">
                    <div className="flex w-full items-center justify-between gap-xs">
                        <InfoPill
                            label="Quiz Solved"
                            value={`${solved ?? mock.solved}/${total}`}
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                        />
                        <InfoPill
                            label="Play time"
                            value={playTime ?? mock.playTime}
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                        />
                        <InfoPill
                            label="Score"
                            value={score ?? mock.score}
                            showLeadingIcon={false}
                            showTrailingIcon={false}
                        />
                        {rank != null && (
                            <InfoPill
                                type="Highlight"
                                label="Rank"
                                value={rank}
                                showLeadingIcon={false}
                                showTrailingIcon={false}
                            />
                        )}
                    </div>
                    {!passed ? (
                        <p className="text-body-default text-center">
                            {passScore != null
                                ? `Score at least ${passScore} points to unlock new stage`
                                : `Solve at least ${unlockRequirement} quizzes to unlock new stage`}
                        </p>
                    ) : canAdvance || isCampaignCleared ? (
                        <ProgressSummaryCard
                            type={
                                isCampaignCleared
                                    ? "Campaign complete"
                                    : "Next stage"
                            }
                            title={
                                isCampaignCleared
                                    ? "All Stages Conquered!"
                                    : nextStageTitle
                            }
                            metaValue={
                                isCampaignCleared
                                    ? campaignScore
                                    : nextStageLevel
                            }
                        />
                    ) : (
                        <p className="text-body-default text-center">
                            Stage complete.
                        </p>
                    )}
                </div>
            }
            footer={
                <div className="flex w-[640px] flex-col gap-lg">
                    <Button
                        label={
                            retryFirst
                                ? "TRY AGAIN"
                                : !canAdvance
                                  ? "VIEW LEADERBOARD"
                                  : "NEXT STAGE"
                        }
                        size="large"
                        className="w-full"
                        leadingIcon={
                            retryFirst ? (
                                <ReplayIcon className="size-full" />
                            ) : !canAdvance ? (
                                <TrophyIcon className="size-full" />
                            ) : undefined
                        }
                        showLeadingIcon={retryFirst || !canAdvance}
                        trailingIcon={
                            <ArrowForwardIcon className="size-full" />
                        }
                        showTrailingIcon={!retryFirst && canAdvance}
                        onClick={
                            retryFirst
                                ? onReplay
                                : !canAdvance
                                  ? onLeaderboard
                                  : onNextStage
                        }
                    />
                    <div className="flex gap-lg">
                        {retryFirst ? (
                            <Button
                                label="NEXT STAGE"
                                size="large"
                                className="flex-1"
                                showLeadingIcon={false}
                                trailingIcon={
                                    <ArrowForwardIcon className="size-full" />
                                }
                                disabled
                            />
                        ) : (
                            <Button
                                label="REPLAY"
                                variant="secondary"
                                size="large"
                                className="flex-1"
                                leadingIcon={
                                    <ReplayIcon className="size-full" />
                                }
                                showTrailingIcon={false}
                                onClick={onReplay}
                            />
                        )}
                        <Button
                            label="QUIT TO MENU"
                            variant="secondary"
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

export default StageResultModal;
