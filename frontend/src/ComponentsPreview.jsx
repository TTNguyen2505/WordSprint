import { useCallback, useEffect, useState } from "react";
import Badge from "./components/Badge.jsx";
import Button from "./components/Button.jsx";
import Cell from "./components/Cell.jsx";
import IconButton from "./components/IconButton.jsx";
import InfoPill from "./components/InfoPill.jsx";
import LeaderboardRecord from "./components/LeaderboardRecord.jsx";
import Modal from "./components/Modal.jsx";
import ModalTitle from "./components/ModalTitle.jsx";
import Navigation from "./components/Navigation.jsx";
import ProfileCard from "./components/ProfileCard.jsx";
import ProfileRecord from "./components/ProfileRecord.jsx";
import ProgressBar from "./components/ProgressBar.jsx";
import ProgressSummaryCard from "./components/ProgressSummaryCard.jsx";
import Quiz from "./components/Quiz.jsx";
import QuizLetter from "./components/QuizLetter.jsx";
import ScoreCard from "./components/ScoreCard.jsx";
import Slider from "./components/Slider.jsx";
import StageCard from "./components/StageCard.jsx";
import Tab from "./components/Tab.jsx";
import TextButton from "./components/TextButton.jsx";
import TextInput from "./components/TextInput.jsx";
import Toggle from "./components/Toggle.jsx";

const sections = [
    ["controls", "Controls"],
    ["feedback", "Feedback"],
    ["game", "Game"],
    ["records", "Records"]
];

const leaderboardData = {
    Overall: [
        { rank: 1, player: "nguyen", score: 150000, playTime: "10:00" },
        {
            rank: 2,
            player: "nguyen_02",
            score: 120000,
            playTime: "10:00",
            isCurrentPlayer: true
        },
        { rank: 3, player: "nguyen_03", score: 95000, playTime: "08:45" }
    ],
    "Case 1": [
        { rank: 1, player: "duy_bao", score: 45000, playTime: "02:15" },
        {
            rank: 2,
            player: "nguyen_02",
            score: 42000,
            playTime: "02:30",
            isCurrentPlayer: true
        }
    ],
    "Case 2": [
        { rank: 1, player: "tuan_khang", score: 62000, playTime: "03:05" }
    ],
    "Case 3": [
        { rank: 1, player: "xuan_tin", score: 78000, playTime: "04:10" }
    ],
    "Case 4": []
};

function Section({ id, number, title, children }) {
    return (
        <section
            id={id}
            className="scroll-mt-24 border-t-2 border-black py-2xl"
        >
            <div className="mb-xl flex flex-wrap items-baseline gap-sm">
                <span className="font-mono text-sm font-bold text-content-muted">
                    {number} /
                </span>
                <h2 className="text-3xl font-bold uppercase">{title}</h2>
            </div>
            <div className="grid min-w-0 grid-cols-[minmax(0,1fr)] gap-2xl">
                {children}
            </div>
        </section>
    );
}

function Specimen({ title, children }) {
    return (
        <div className="grid min-w-0 grid-cols-[minmax(0,1fr)] gap-md lg:grid-cols-[180px_minmax(0,1fr)] lg:gap-xl">
            <h3 className="font-mono text-sm font-bold uppercase">{title}</h3>
            <div className="min-w-0">{children}</div>
        </div>
    );
}

function Sample({ label, children }) {
    return (
        <div className="grid min-w-0 grid-cols-[minmax(0,1fr)] content-start gap-sm">
            <span className="font-mono text-xs font-bold uppercase text-content-muted">
                {label}
            </span>
            {children}
        </div>
    );
}

function ComponentsPreview() {
    const [toggle, setToggle] = useState(true);
    const [sliderEnabled, setSliderEnabled] = useState(true);
    const [sliderValue, setSliderValue] = useState(60);
    const [activeTab, setActiveTab] = useState("Overall");
    const [records, setRecords] = useState(leaderboardData.Overall);
    const [currentUserId, setCurrentUserId] = useState("nguyen_02");
    const [modalOpen, setModalOpen] = useState(false);
    const [quizMode, setQuizMode] = useState("scramble");
    const [quizAnswer, setQuizAnswer] = useState("");
    const [quizResult, setQuizResult] = useState("");
    const [playsRemaining, setPlaysRemaining] = useState(3);

    const renderLeaderboard = useCallback((data, userId) => {
        if (userId !== undefined) setCurrentUserId(userId);
        setRecords(Array.isArray(data) ? [...data] : []);
    }, []);

    const handleTabChange = useCallback(
        (tab) => {
            setActiveTab(tab);
            if (typeof window.onTabChange === "function")
                window.onTabChange(tab);
            renderLeaderboard(leaderboardData[tab] || []);
        },
        [renderLeaderboard]
    );

    const closeModal = useCallback(() => {
        setModalOpen(false);
        if (typeof window.onClose === "function") window.onClose();
    }, []);

    useEffect(() => {
        window.renderLeaderboard = renderLeaderboard;
        window.setLeaderboardTab = handleTabChange;
        return () => {
            delete window.renderLeaderboard;
            delete window.setLeaderboardTab;
        };
    }, [renderLeaderboard, handleTabChange]);

    const changeQuizMode = (mode) => {
        setQuizMode(mode);
        setQuizAnswer("");
        setQuizResult("");
        setPlaysRemaining(3);
    };

    return (
        <main className="min-h-screen bg-surface-canvas font-sans text-content-primary">
            <header className="border-b-2 border-black bg-white">
                <div className="mx-auto flex max-w-[1440px] flex-wrap items-center justify-between gap-md px-md py-md sm:px-xl">
                    <a
                        href="#top"
                        className="font-mono text-sm font-bold uppercase"
                    >
                        WordSprint{" "}
                        <span className="text-content-muted">/ UI library</span>
                    </a>
                    <span className="bg-surface-primary-cta px-xs py-2xs font-mono text-xs font-bold uppercase">
                        Component review
                    </span>
                </div>
            </header>

            <div id="top" className="mx-auto max-w-[1440px] px-md sm:px-xl">
                <div className="flex flex-wrap items-end justify-between gap-lg py-2xl">
                    <div>
                        <p className="mb-sm font-mono text-xs font-bold uppercase text-content-muted">
                            WordSprint / Design system
                        </p>
                        <h1 className="text-4xl font-bold uppercase sm:text-5xl">
                            Component index
                            <span className="text-content-highlight">.</span>
                        </h1>
                    </div>
                    <p className="font-mono text-xs font-bold uppercase">
                        21 components{" "}
                        <span className="mx-xs text-content-muted">/</span> 04
                        categories
                    </p>
                </div>

                <nav
                    aria-label="Component categories"
                    className="sticky top-0 z-10 flex flex-wrap gap-xs border-y-2 border-black bg-surface-canvas py-sm"
                >
                    {sections.map(([id, label], index) => (
                        <a
                            key={id}
                            href={`#${id}`}
                            className="border border-black bg-white px-sm py-xs font-mono text-xs font-bold uppercase hover:bg-surface-primary-cta focus-visible:bg-surface-primary-cta"
                        >
                            <span className="mr-xs text-content-muted">
                                0{index + 1}
                            </span>
                            {label}
                        </a>
                    ))}
                </nav>

                <Section id="controls" number="01" title="Controls">
                    <Specimen title="Button">
                        <div className="grid gap-lg">
                            <div className="flex flex-wrap items-end gap-lg">
                                {[
                                    "primary",
                                    "secondary",
                                    "destructive",
                                    "inverse",
                                    "disabled"
                                ].map((variant) => (
                                    <Sample key={variant} label={variant}>
                                        <Button
                                            label="Button"
                                            variant={variant}
                                            size="default"
                                            showTrailingIcon={false}
                                        />
                                    </Sample>
                                ))}
                            </div>
                            <div className="flex flex-wrap items-end gap-lg">
                                {["default", "hover", "pressed"].map(
                                    (state) => (
                                        <Sample key={state} label={state}>
                                            <Button
                                                label="Button"
                                                state={state}
                                                size="default"
                                                showLeadingIcon={false}
                                                showTrailingIcon={false}
                                            />
                                        </Sample>
                                    )
                                )}
                                {["small", "large"].map((size) => (
                                    <Sample key={size} label={size}>
                                        <Button
                                            label="Button"
                                            size={size}
                                            showTrailingIcon={false}
                                        />
                                    </Sample>
                                ))}
                            </div>
                        </div>
                    </Specimen>
                    <Specimen title="IconButton">
                        <div className="grid gap-lg">
                            {["Small", "Extra Small"].map((size) => (
                                <div
                                    key={size}
                                    className="flex flex-wrap gap-lg"
                                >
                                    {[
                                        "Default",
                                        "Hover",
                                        "Pressed",
                                        "Disabled"
                                    ].map((state) => (
                                        <Sample
                                            key={state}
                                            label={`${size} / ${state}`}
                                        >
                                            <IconButton
                                                size={size}
                                                state={state}
                                                ariaLabel={`${size} ${state} icon button`}
                                            />
                                        </Sample>
                                    ))}
                                </div>
                            ))}
                        </div>
                    </Specimen>
                    <Specimen title="TextButton">
                        <div className="flex flex-wrap gap-xl">
                            <Sample label="Default">
                                <TextButton label="View details" />
                            </Sample>
                            <Sample label="Hover">
                                <TextButton
                                    label="View details"
                                    state="hover"
                                />
                            </Sample>
                        </div>
                    </Specimen>
                    <Specimen title="TextInput">
                        <div className="grid gap-lg md:grid-cols-2">
                            {["default", "focus", "error", "success"].map(
                                (state) => (
                                    <Sample key={state} label={state}>
                                        <TextInput
                                            title="Player name"
                                            placeholder="Enter a name"
                                            state={state}
                                            defaultValue={
                                                state === "default"
                                                    ? ""
                                                    : "word_sprinter"
                                            }
                                            supportingText={
                                                state === "error"
                                                    ? "This name is already taken."
                                                    : state === "success"
                                                      ? "This name is available."
                                                      : undefined
                                            }
                                        />
                                    </Sample>
                                )
                            )}
                            {["error", "success"].map((state) => (
                                <Sample
                                    key={`compact-${state}`}
                                    label={`Compact ${state}`}
                                >
                                    <TextInput
                                        state={state}
                                        defaultValue="word_sprinter"
                                        showTitle={false}
                                        showSupportingText={false}
                                        aria-label={`Compact ${state} player name`}
                                    />
                                </Sample>
                            ))}
                            <Sample label="Disabled">
                                <TextInput
                                    title="Player name"
                                    defaultValue="word_sprinter"
                                    disabled
                                />
                            </Sample>
                        </div>
                    </Specimen>
                    <Specimen title="Toggle">
                        <div className="flex flex-wrap gap-xl">
                            <Sample label="Interactive">
                                <Toggle
                                    enabled={toggle}
                                    onChange={setToggle}
                                    ariaLabel="Interactive toggle"
                                />
                            </Sample>
                            <Sample label="Off">
                                <Toggle ariaLabel="Off toggle" />
                            </Sample>
                            <Sample label="On">
                                <Toggle enabled ariaLabel="On toggle" />
                            </Sample>
                        </div>
                    </Specimen>
                    <Specimen title="Slider">
                        <div className="grid grid-cols-[minmax(0,1fr)] gap-lg lg:grid-cols-2">
                            <Sample label={`Interactive / ${sliderValue}`}>
                                <Slider
                                    title="Sound"
                                    enabled={sliderEnabled}
                                    onToggle={setSliderEnabled}
                                    value={sliderValue}
                                    onChange={setSliderValue}
                                />
                            </Sample>
                            <Sample label="Off">
                                <Slider
                                    title="Music"
                                    enabled={false}
                                    value={35}
                                />
                            </Sample>
                        </div>
                    </Specimen>
                    <Specimen title="Tab & Navigation">
                        <div className="grid gap-lg">
                            <div
                                className="flex flex-wrap gap-xs"
                                role="tablist"
                                aria-label="Tab states"
                            >
                                <Tab label="Default" />
                                <Tab label="Active" active />
                                <Tab
                                    label="Hover"
                                    className="bg-surface-canvas"
                                />
                            </div>
                            <div className="overflow-x-auto pb-xs">
                                <Navigation
                                    items={[
                                        "Case 1",
                                        "Case 2",
                                        "Case 3",
                                        "Case 4",
                                        "Overall"
                                    ]}
                                    activeTab={activeTab}
                                    onChange={handleTabChange}
                                />
                            </div>
                        </div>
                    </Specimen>
                </Section>

                <Section id="feedback" number="02" title="Feedback">
                    <Specimen title="Badge">
                        <div className="flex flex-wrap gap-lg">
                            {[
                                "default",
                                "pink",
                                "green",
                                "red",
                                "black",
                                "disabled"
                            ].map((variant) => (
                                <Sample key={variant} label={variant}>
                                    <Badge
                                        variant={variant}
                                        disabled={variant === "disabled"}
                                        label={variant}
                                    />
                                </Sample>
                            ))}
                        </div>
                    </Specimen>
                    <Specimen title="InfoPill">
                        <div className="flex flex-wrap gap-lg">
                            {[
                                "default",
                                "pink",
                                "green",
                                "red",
                                "black",
                                "disabled"
                            ].map((variant) => (
                                <Sample key={variant} label={variant}>
                                    <InfoPill
                                        variant={variant}
                                        disabled={variant === "disabled"}
                                        label="Level"
                                        value="A2"
                                        showTrailingIcon={false}
                                    />
                                </Sample>
                            ))}
                        </div>
                    </Specimen>
                    <Specimen title="ProgressBar">
                        <div className="grid gap-lg md:grid-cols-3">
                            <Sample label="Empty">
                                <ProgressBar
                                    value={0}
                                    statusLabel="Not started"
                                    metricLabel="0 / 12"
                                />
                            </Sample>
                            <Sample label="In progress">
                                <ProgressBar
                                    value={6}
                                    max={12}
                                    statusLabel="In progress"
                                    metricLabel="6 / 12"
                                />
                            </Sample>
                            <Sample label="Completed">
                                <ProgressBar
                                    value={12}
                                    max={12}
                                    statusLabel="Completed"
                                    metricLabel="12 / 12"
                                />
                            </Sample>
                        </div>
                    </Specimen>
                    <Specimen title="Modal & ModalTitle">
                        <div className="flex flex-wrap items-center gap-lg">
                            <ModalTitle as="h4" title="Modal title" />
                            <Button
                                label="Open modal"
                                size="default"
                                showTrailingIcon={false}
                                onClick={() => setModalOpen(true)}
                            />
                        </div>
                        <Modal
                            isOpen={modalOpen}
                            onClose={closeModal}
                            showCloseButton
                            header={<ModalTitle title="Ready to play?" />}
                            footer={
                                <Button
                                    label="Close"
                                    variant="secondary"
                                    size="default"
                                    showTrailingIcon={false}
                                    onClick={closeModal}
                                />
                            }
                        >
                            <div className="flex h-full items-center justify-center">
                                <p>Your next word is waiting.</p>
                            </div>
                        </Modal>
                    </Specimen>
                </Section>

                <Section id="game" number="03" title="Game">
                    <Specimen title="QuizLetter">
                        <div className="flex flex-wrap gap-lg">
                            <Sample label="Empty">
                                <QuizLetter blank />
                            </Sample>
                            <Sample label="Letter">
                                <QuizLetter letter="A" />
                            </Sample>
                        </div>
                    </Specimen>
                    <Specimen title="StageCard">
                        <div className="grid gap-lg md:grid-cols-3">
                            {["solved", "available", "locked"].map(
                                (status, index) => (
                                    <Sample key={status} label={status}>
                                        <StageCard
                                            status={status}
                                            stageId={`Stage 0${index + 1}`}
                                            topic={
                                                [
                                                    "Everyday",
                                                    "Family",
                                                    "Travel"
                                                ][index]
                                            }
                                            score={
                                                status === "solved" ? 1250 : 0
                                            }
                                        />
                                    </Sample>
                                )
                            )}
                        </div>
                    </Specimen>
                    <Specimen title="ProgressSummaryCard">
                        <div className="grid gap-lg lg:grid-cols-2">
                            <Sample label="Available">
                                <ProgressSummaryCard type="Next stage" />
                            </Sample>
                            <Sample label="Completed">
                                <ProgressSummaryCard type="Campaign complete" />
                            </Sample>
                        </div>
                    </Specimen>
                    <Specimen title="Quiz">
                        <div className="grid gap-md">
                            <div
                                className="flex flex-wrap gap-xs"
                                role="tablist"
                                aria-label="Quiz mode"
                            >
                                {["scramble", "fill-blank", "audio"].map(
                                    (mode) => (
                                        <Tab
                                            key={mode}
                                            label={mode}
                                            active={quizMode === mode}
                                            onClick={() => changeQuizMode(mode)}
                                        />
                                    )
                                )}
                            </div>
                            <Quiz
                                mode={quizMode}
                                answer={quizAnswer}
                                onChange={(value) => {
                                    setQuizAnswer(value);
                                    setQuizResult("");
                                }}
                                onSubmit={(value) =>
                                    setQuizResult(
                                        value.toLowerCase() === "apple"
                                            ? "Correct!"
                                            : "Try again."
                                    )
                                }
                                hasPrevious={false}
                                hasNext={false}
                                playsRemaining={playsRemaining}
                                onPlaySound={() => {
                                    setPlaysRemaining((remaining) =>
                                        Math.max(remaining - 1, 0)
                                    );
                                    if ("speechSynthesis" in window)
                                        window.speechSynthesis.speak(
                                            new SpeechSynthesisUtterance(
                                                "apple"
                                            )
                                        );
                                }}
                            />
                            {quizResult && (
                                <p
                                    role="status"
                                    className="font-mono text-sm font-bold"
                                >
                                    {quizResult}
                                </p>
                            )}
                        </div>
                    </Specimen>
                </Section>

                <Section id="records" number="04" title="Records">
                    <Specimen title="ProfileCard & ScoreCard">
                        <div className="flex flex-wrap gap-lg">
                            <ProfileCard />
                            <ScoreCard
                                onAction={() =>
                                    document
                                        .getElementById("leaderboard")
                                        ?.scrollIntoView()
                                }
                            />
                        </div>
                    </Specimen>
                    <Specimen title="Cell">
                        <div className="flex flex-wrap gap-md">
                            {[
                                {
                                    type: "Text",
                                    label: "Text",
                                    showBadge: false
                                },
                                {
                                    type: "Text",
                                    label: "Text + Badge",
                                    showBadge: true
                                },
                                {
                                    type: "Numerical",
                                    label: "Numerical",
                                    data: "01"
                                },
                                {
                                    type: "Header",
                                    label: "Header",
                                    data: "PLAYER"
                                },
                                { type: "Rank", label: "Rank", data: "01" },
                                { type: "Action", label: "Action" }
                            ].map(({ type, label, showBadge, data }) => (
                                <Sample key={label} label={label}>
                                    <Cell
                                        type={type}
                                        data={data ?? "Player"}
                                        showBadge={showBadge}
                                        badgeLabel="YOU"
                                        actionLabel="Delete profile"
                                    />
                                </Sample>
                            ))}
                        </div>
                    </Specimen>
                    <Specimen title="ProfileRecord">
                        <div className="max-w-[600px] overflow-x-auto">
                            <div className="min-w-[520px]">
                                <ProfileRecord type="Header" />
                                <ProfileRecord
                                    type="Active"
                                    playerName="nguyen_02"
                                    progress="7/12"
                                />
                                <ProfileRecord
                                    type="Default"
                                    playerName="nguyen_03"
                                />
                                <ProfileRecord
                                    type="Default"
                                    state="Hover"
                                    playerName="nguyen_03"
                                />
                            </div>
                        </div>
                    </Specimen>
                    <Specimen title="LeaderboardRecord">
                        <div
                            id="leaderboard"
                            className="scroll-mt-24 overflow-x-auto pb-xs"
                        >
                            <div className="min-w-[560px] max-w-[700px] border border-black bg-white">
                                <LeaderboardRecord variant="header" />
                                {records.length === 0 ? (
                                    <p className="p-xl text-center font-mono text-sm font-bold uppercase text-content-muted">
                                        No records yet
                                    </p>
                                ) : (
                                    records.map((item, index) => {
                                        const isYou =
                                            item.isCurrentPlayer === true ||
                                            (currentUserId &&
                                                String(item.player).trim() ===
                                                    String(
                                                        currentUserId
                                                    ).trim());
                                        const isChampion =
                                            Number(item.rank) === 1;
                                        return (
                                            <LeaderboardRecord
                                                key={`${item.rank}-${item.player}-${index}`}
                                                variant={
                                                    isYou
                                                        ? "you"
                                                        : isChampion
                                                          ? "champion"
                                                          : "default"
                                                }
                                                rank={item.rank}
                                                playerName={item.player}
                                                score={item.score}
                                                playTime={item.playTime}
                                                badgeLabel={
                                                    isChampion
                                                        ? "CHAMPION"
                                                        : undefined
                                                }
                                            />
                                        );
                                    })
                                )}
                            </div>
                        </div>
                    </Specimen>
                </Section>
                <footer className="flex flex-wrap justify-between gap-sm border-t-2 border-black py-lg font-mono text-xs font-bold uppercase">
                    <span>WordSprint / UI library</span>
                    <a href="#top" className="underline">
                        Back to top
                    </a>
                </footer>
            </div>
        </main>
    );
}

export default ComponentsPreview;
