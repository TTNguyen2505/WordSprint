import { useRef } from "react";
import ArrowBackIcon from "../assets/icons/arrow_back_1.svg?react";
import ArrowForwardIcon from "../assets/icons/arrow_forward_1.svg?react";
import ExitIcon from "../assets/icons/exit.svg?react";
import HelpIcon from "../assets/icons/help.svg?react";
import SettingsIcon from "../assets/icons/settings.svg?react";
import stagePreview from "../assets/menu-stage-preview.png";
import Button from "../components/Button";
import IconButton from "../components/IconButton";
import ProfileCard from "../components/ProfileCard";
import ProgressBar from "../components/ProgressBar";
import ScoreCard from "../components/ScoreCard";
import StageCard from "../components/StageCard";
import sampleStages from "../sampleStages";

function MenuScreen({
    username = "TTNguyen2505",
    score = 124850,
    stages = sampleStages,
    onSwitchProfile,
    onLeaderboard,
    onPlay,
    onHelp,
    onSettings,
    onExit,
    onStageAction
}) {
    const stagesRef = useRef(null);
    const solvedCount = stages.filter(
        (stage) => stage.status === "solved"
    ).length;

    const scrollStages = (direction) => {
        const container = stagesRef.current;
        const firstCard = container?.firstElementChild;
        if (!container || !firstCard) return;

        const gap = parseFloat(getComputedStyle(container).columnGap) || 0;
        container.scrollBy({
            left: direction * (firstCard.getBoundingClientRect().width + gap),
            behavior: "smooth"
        });
    };

    return (
        <main className="flex h-[900px] w-[1200px] flex-col justify-between bg-surface-canvas px-xl py-lg text-content-primary">
            <header className="flex items-start justify-between">
                <ProfileCard
                    username={username}
                    onSwitchProfile={onSwitchProfile}
                />
                <ScoreCard score={score} onAction={onLeaderboard} />
            </header>

            <section className="mx-auto flex w-[600px] flex-col items-center gap-xl text-center">
                <div>
                    <h1 className="text-display-hero">SCRAMBLY</h1>
                    <p className="text-subtitle-default">
                        Master English vocabulary effortlessly with interactive
                        word puzzles and audio quizzes. Beat the timer, use
                        smart power-ups, and challenge your friends for the top
                        spot on the leaderboard!
                    </p>
                </div>
                <div className="flex w-[408px] flex-col gap-md">
                    <Button
                        className="w-full"
                        label="PLAY"
                        size="large"
                        showTrailingIcon={false}
                        onClick={onPlay}
                    />
                    <div className="flex justify-center gap-md">
                        <Button
                            label="Help"
                            size="default"
                            variant="secondary"
                            leadingIcon={<HelpIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={onHelp}
                        />
                        <Button
                            label="Settings"
                            size="default"
                            variant="secondary"
                            leadingIcon={<SettingsIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={onSettings}
                        />
                        <Button
                            label="Exit game"
                            size="default"
                            variant="destructive"
                            leadingIcon={<ExitIcon className="size-full" />}
                            showTrailingIcon={false}
                            onClick={onExit}
                        />
                    </div>
                </div>
            </section>

            <section aria-label="Stage selection">
                <div className="mb-md flex items-end justify-between gap-md">
                    <ProgressBar
                        value={solvedCount}
                        max={Math.max(stages.length, 1)}
                        statusLabel="YOUR CURRENT PROGRESS"
                        metricLabel={`${solvedCount}/${stages.length} SOLVED`}
                    />
                    <div className="flex shrink-0 gap-xs">
                        <IconButton
                            ariaLabel="Previous stages"
                            icon={<ArrowBackIcon className="size-full" />}
                            onClick={() => scrollStages(-1)}
                        />
                        <IconButton
                            ariaLabel="Next stages"
                            icon={<ArrowForwardIcon className="size-full" />}
                            onClick={() => scrollStages(1)}
                        />
                    </div>
                </div>
                <div
                    ref={stagesRef}
                    className="flex gap-lg overflow-x-auto pb-xs [scrollbar-width:none] [&::-webkit-scrollbar]:hidden"
                    aria-label="Stages"
                >
                    {stages.map((stage) => (
                        <StageCard
                            key={stage.stageId}
                            {...stage}
                            className="shrink-0"
                            imageSrc={stage.imageSrc ?? stagePreview}
                            imageAlt={stage.imageAlt ?? ""}
                            onAction={
                                onStageAction
                                    ? () => onStageAction(stage)
                                    : undefined
                            }
                        />
                    ))}
                </div>
            </section>
        </main>
    );
}

export default MenuScreen;
