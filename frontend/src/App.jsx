import { useState, useEffect, useCallback } from "react";
import Modal from "./components/Modal.jsx";
import ModalTitle from "./components/ModalTitle.jsx";
import Navigation from "./components/Navigation.jsx";
import LeaderboardRecord from "./components/LeaderboardRecord.jsx";
import InfoPill from "./components/InfoPill.jsx";
import Badge from "./components/Badge.jsx";
import Button from "./components/Button.jsx";
import StageCard from "./components/StageCard.jsx";
import ProgressSummaryCard from "./components/ProgressSummaryCard.jsx";
import ProfileCard from "./components/ProfileCard.jsx";

import heroImg from "./assets/hero.png";
import ReplayIcon from "./assets/icons/replay.svg?react";
import ExitIcon from "./assets/icons/exit.svg?react";
import TrophyIcon from "./assets/icons/trophy.svg?react";
import ArrowForwardIcon from "./assets/icons/arrow_forward_1.svg?react";
import CheckIcon from "./assets/icons/check.svg?react";
import CloseIcon from "./assets/icons/close.svg?react";
import AddIcon from "./assets/icons/add.svg?react";
import AccountIcon from "./assets/icons/account.svg?react";

// Mock datasets for the 5 tabs in Hall of Fame modal (OOP-50)
const DEFAULT_LEADERBOARD_DATA = {
    Overall: [
        {
            rank: 1,
            player: "nguyen",
            score: 100000,
            playTime: "10:00",
            isCurrentPlayer: false
        },
        {
            rank: 2,
            player: "nguyen_02",
            score: 150000,
            playTime: "10:00",
            isCurrentPlayer: true
        },
        {
            rank: 3,
            player: "nguyen_03",
            score: 120000,
            playTime: "10:00",
            isCurrentPlayer: false
        },
        {
            rank: 4,
            player: "nguyen_04",
            score: 120000,
            playTime: "10:00",
            isCurrentPlayer: false
        },
        {
            rank: 5,
            player: "nguyen_05",
            score: 110000,
            playTime: "09:30",
            isCurrentPlayer: false
        },
        {
            rank: 6,
            player: "nguyen_06",
            score: 95000,
            playTime: "08:45",
            isCurrentPlayer: false
        },
        {
            rank: 7,
            player: "nguyen_07",
            score: 90000,
            playTime: "08:20",
            isCurrentPlayer: false
        }
    ],
    "Case 1": [
        {
            rank: 1,
            player: "duy_bao",
            score: 45000,
            playTime: "02:15",
            isCurrentPlayer: false
        },
        {
            rank: 2,
            player: "nguyen_02",
            score: 42000,
            playTime: "02:30",
            isCurrentPlayer: true
        },
        {
            rank: 3,
            player: "van_dai",
            score: 39000,
            playTime: "02:45",
            isCurrentPlayer: false
        },
        {
            rank: 4,
            player: "tuan_khang",
            score: 36000,
            playTime: "03:10",
            isCurrentPlayer: false
        },
        {
            rank: 5,
            player: "xuan_tin",
            score: 31000,
            playTime: "03:40",
            isCurrentPlayer: false
        }
    ],
    "Case 2": [
        {
            rank: 1,
            player: "tuan_khang",
            score: 62000,
            playTime: "03:05",
            isCurrentPlayer: false
        },
        {
            rank: 2,
            player: "van_dai",
            score: 58000,
            playTime: "03:22",
            isCurrentPlayer: false
        },
        {
            rank: 3,
            player: "duy_bao",
            score: 54000,
            playTime: "03:40",
            isCurrentPlayer: false
        },
        {
            rank: 4,
            player: "nguyen_02",
            score: 51000,
            playTime: "04:00",
            isCurrentPlayer: true
        }
    ],
    "Case 3": [
        {
            rank: 1,
            player: "xuan_tin",
            score: 78000,
            playTime: "04:10",
            isCurrentPlayer: false
        },
        {
            rank: 2,
            player: "nguyen_02",
            score: 75000,
            playTime: "04:25",
            isCurrentPlayer: true
        },
        {
            rank: 3,
            player: "duy_bao",
            score: 71000,
            playTime: "04:50",
            isCurrentPlayer: false
        }
    ],
    "Case 4": [] // Demonstrates required empty state
};

const TAB_ITEMS = ["Case 1", "Case 2", "Case 3", "Case 4", "Overall"];

// Default campaign stages for the Main Menu (Stage Select)
const DEFAULT_CAMPAIGN_STAGES = [
  {
    id: 1,
    stageId: "STAGE 1",
    topic: "School",
    level: "A1",
    status: "solved",
    score: 150,
  },
  {
    id: 2,
    stageId: "STAGE 2",
    topic: "Family",
    level: "A2",
    status: "solved",
    score: 250,
  },
  {
    id: 3,
    stageId: "STAGE 3",
    topic: "Work",
    level: "B1",
    status: "solved",
    score: 300,
  },
  {
    id: 4,
    stageId: "STAGE 4",
    topic: "Travel",
    level: "B2",
    status: "solved",
    score: 300,
  },
  {
    id: 5,
    stageId: "STAGE 5",
    topic: "School",
    level: "C1",
    status: "solved",
    score: 450,
  },
];

// Default initial simulation state for Stage Result (Figma Frame 1: Time's up / Failed)
const DEFAULT_STAGE_RESULT = {
  stageId: 1,
  stageName: "STAGE 1: SCHOOL",
  solved: 3,
  totalQuiz: 8,
  playTime: 80, // 80s -> 1:20
  score: 150,
  rank: null,
  campaignScore: 1450,
  unlockThreshold: 4,
  isAllCleared: false,
};

function formatPlayTime(secondsOrStr) {
  if (typeof secondsOrStr === "string") return secondsOrStr;
  if (typeof secondsOrStr === "number") {
    const mins = Math.floor(secondsOrStr / 60);
    const secs = secondsOrStr % 60;
    return `${mins}:${secs < 10 ? "0" : ""}${secs}`;
  }
  return "0:00";
}

function App() {
    const [isOpen, setIsOpen] = useState(true);
    const [activeTab, setActiveTab] = useState("Overall");
    const [currentUserId, setCurrentUserId] = useState("nguyen_02");
    const [records, setRecords] = useState(DEFAULT_LEADERBOARD_DATA["Overall"]);

    /**
     * Flexible function to render leaderboard data dynamically.
     * Can be invoked from external code/scripts via window.renderLeaderboard.
     */
    const renderLeaderboard = useCallback((data, userId) => {
        if (userId !== undefined) {
            setCurrentUserId(userId);
        }
        setRecords(Array.isArray(data) ? [...data] : []);
    }, []);

    /**
     * Tab switch handler calling onTabChange callback.
     */
    const handleTabChange = useCallback(
        (tabName) => {
            setActiveTab(tabName);

            // Call external callback if registered
            if (typeof window.onTabChange === "function") {
                window.onTabChange(tabName);
            }

            // Load corresponding tab dataset
            const nextData = DEFAULT_LEADERBOARD_DATA[tabName] || [];
            renderLeaderboard(nextData);
        },
        [renderLeaderboard]
    );

    const handleClose = useCallback(() => {
        setIsOpen(false);
        if (typeof window.onClose === "function") {
            window.onClose();
        }
    }, []);

    // Expose global methods for external modules/tests
    useEffect(() => {
        window.renderLeaderboard = renderLeaderboard;
        window.setLeaderboardTab = handleTabChange;
        return () => {
            delete window.renderLeaderboard;
            delete window.setLeaderboardTab;
        };
    }, [renderLeaderboard, handleTabChange]);

    return (
        <main className="flex min-h-screen w-full items-center justify-center bg-surface-canvas p-md font-sans text-content-primary">
            {/* Re-open button if modal is closed */}
            {!isOpen && (
                <Button
                    label="Open Leaderboard"
                    variant="primary"
                    size="default"
                    onClick={() => setIsOpen(true)}
                    className="shadow-elevation-200"
                />
            )}

            {/* HALL OF FAME Leaderboard Modal */}
            <Modal
                isOpen={isOpen}
                onClose={handleClose}
                maxWidth="max-w-[752px]"
                className="w-[752px] min-h-[auto] max-h-[92vh] border-2 border-stroke-default p-xl sm:p-2xl shadow-elevation-200 flex flex-col items-center gap-xl"
                overlayClassName="bg-black/50 backdrop-blur-20"
                closeOnBackdropClick={true}
                closeOnEscape={true}
            >
                {/* 1. Header with Title */}
                <header className="flex w-full items-center justify-center">
                    <ModalTitle
                        as="h1"
                        className="text-center font-sans text-4xl font-bold uppercase tracking-tight text-content-primary"
                    >
                        HALL OF FAME
                    </ModalTitle>
                </header>

                {/* 2. Navigation Tabs Bar */}
                <section className="flex w-full justify-center">
                    <Navigation
                        items={TAB_ITEMS}
                        activeTab={activeTab}
                        onChange={handleTabChange}
                        className="w-[652px] max-w-full border-2 border-stroke-default p-1.5 shadow-elevation-200"
                    />
                </section>

                {/* 3. Leaderboard Table Container */}
                <section className="flex w-full justify-center">
                    <div className="w-[600px] max-w-full border border-stroke-default bg-surface-panel flex flex-col">
                        {/* Header Row */}
                        <LeaderboardRecord
                            variant="header"
                            rank="RANK"
                            playerName="PLAYER"
                            score="SCORE"
                            playTime="PLAY TIME"
                        />

                        {/* Dynamic Data Rows Body */}
                        <div
                            id="leaderboard-table-body"
                            className="max-h-[336px] overflow-y-auto flex flex-col divide-y divide-stroke-default"
                        >
                            {records.length === 0 ? (
                                <div className="flex h-36 items-center justify-center font-sans text-base font-bold text-content-muted uppercase tracking-wider">
                                    Chưa có dữ liệu
                                </div>
                            ) : (
                                records.map((item, index) => {
                                    const rankNum =
                                        typeof item.rank === "number"
                                            ? item.rank
                                            : parseInt(item.rank, 10);
                                    const isTop1 =
                                        rankNum === 1 ||
                                        item.rank === "01" ||
                                        item.rank === "1" ||
                                        (index === 0 && item.rank == 1);
                                    const isYou = Boolean(
                                        item.isCurrentPlayer === true ||
                                        (currentUserId &&
                                            String(item.player).trim() ===
                                                String(currentUserId).trim())
                                    );

                                    let variant = "default";
                                    if (isTop1) variant = "champion";
                                    if (isYou) variant = "you";

                                    return (
                                        <LeaderboardRecord
                                            key={`${item.rank}-${item.player}-${index}`}
                                            variant={variant}
                                            rank={item.rank}
                                            playerName={item.player}
                                            score={item.score}
                                            playTime={item.playTime}
                                            badgeLabel={
                                                isTop1
                                                    ? "CHAMPION"
                                                    : isYou
                                                      ? "YOU"
                                                      : undefined
                                            }
                                        />
                                    );
                                })
                            )}
                        </div>
                    </div>
                </section>

                {/* 4. Footer with Close Button */}
                <footer className="flex w-full items-center justify-center pt-xs">
                    <Button
                        label="Close"
                        variant="secondary"
                        size="default"
                        leadingIcon={<CloseIcon className="size-3.5" />}
                        showTrailingIcon={false}
                        onClick={handleClose}
                        className="w-[120px] h-[40px] border-2 border-stroke-default shadow-elevation-200 text-sm font-bold"
                    />
                </footer>
            </Modal>
        </main>
    );
}

export default App;
