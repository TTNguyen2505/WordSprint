import { useState, useEffect, useCallback, useMemo } from "react";
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
    { rank: 1, player: "nguyen", score: 100000, playTime: "10:00", isCurrentPlayer: false },
    { rank: 2, player: "nguyen_02", score: 150000, playTime: "10:00", isCurrentPlayer: true },
    { rank: 3, player: "nguyen_03", score: 120000, playTime: "10:00", isCurrentPlayer: false },
    { rank: 4, player: "nguyen_04", score: 120000, playTime: "10:00", isCurrentPlayer: false },
    { rank: 5, player: "nguyen_05", score: 110000, playTime: "09:30", isCurrentPlayer: false },
    { rank: 6, player: "nguyen_06", score: 95000, playTime: "08:45", isCurrentPlayer: false },
    { rank: 7, player: "nguyen_07", score: 90000, playTime: "08:20", isCurrentPlayer: false },
  ],
  "Case 1": [
    { rank: 1, player: "duy_bao", score: 45000, playTime: "02:15", isCurrentPlayer: false },
    { rank: 2, player: "nguyen_02", score: 42000, playTime: "02:30", isCurrentPlayer: true },
    { rank: 3, player: "van_dai", score: 39000, playTime: "02:45", isCurrentPlayer: false },
    { rank: 4, player: "tuan_khang", score: 36000, playTime: "03:10", isCurrentPlayer: false },
    { rank: 5, player: "xuan_tin", score: 31000, playTime: "03:40", isCurrentPlayer: false },
  ],
  "Case 2": [
    { rank: 1, player: "tuan_khang", score: 62000, playTime: "03:05", isCurrentPlayer: false },
    { rank: 2, player: "van_dai", score: 58000, playTime: "03:22", isCurrentPlayer: false },
    { rank: 3, player: "duy_bao", score: 54000, playTime: "03:40", isCurrentPlayer: false },
    { rank: 4, player: "nguyen_02", score: 51000, playTime: "04:00", isCurrentPlayer: true },
  ],
  "Case 3": [
    { rank: 1, player: "xuan_tin", score: 78000, playTime: "04:10", isCurrentPlayer: false },
    { rank: 2, player: "nguyen_02", score: 75000, playTime: "04:25", isCurrentPlayer: true },
    { rank: 3, player: "duy_bao", score: 71000, playTime: "04:50", isCurrentPlayer: false },
  ],
  "Case 4": [],
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
  const [isResultOpen, setIsResultOpen] = useState(true);
  const [isLeaderboardOpen, setIsLeaderboardOpen] = useState(false);
  const [stageResult, setStageResult] = useState(DEFAULT_STAGE_RESULT);

  // Leaderboard state
  const [activeTab, setActiveTab] = useState("Overall");
  const [currentUserId, setCurrentUserId] = useState("nguyen_02");
  const [records, setRecords] = useState(DEFAULT_LEADERBOARD_DATA["Overall"]);

  // Dynamic conditions
  const isAllCleared = Boolean(stageResult.isAllCleared);
  const isCleared = stageResult.isCleared ?? (stageResult.solved >= (stageResult.unlockThreshold ?? 4));
  const nextStageUnlocked = stageResult.nextStageUnlocked ?? isCleared;
  const isFailed = !isCleared && !isAllCleared;

  /**
   * Flexible function to receive dynamic stage result data.
   * Exposed as window.renderStageResult and window.updateStageResult.
   */
  const updateStageResult = useCallback((data) => {
    if (!data) return;
    setStageResult((prev) => ({
      ...prev,
      ...data,
    }));
  }, []);

  /**
   * Flexible function to render leaderboard data dynamically.
   */
  const renderLeaderboard = useCallback((data, userId) => {
    if (userId !== undefined) {
      setCurrentUserId(userId);
    }
    setRecords(Array.isArray(data) ? [...data] : []);
  }, []);

  const handleTabChange = useCallback((tabName) => {
    setActiveTab(tabName);
    if (typeof window.onTabChange === "function") {
      window.onTabChange(tabName);
    }
    const nextData = DEFAULT_LEADERBOARD_DATA[tabName] || [];
    renderLeaderboard(nextData);
  }, [renderLeaderboard]);

  // Stage Result Action handlers
  const handleTryAgainOrReplay = useCallback(() => {
    if (typeof window.onReplayGame === "function") {
      window.onReplayGame(stageResult);
    }
    window.dispatchEvent(
      new CustomEvent("game-replay", { detail: stageResult })
    );
  }, [stageResult]);

  const handleNextStage = useCallback(() => {
    if (!nextStageUnlocked) return;
    if (typeof window.onNextStage === "function") {
      window.onNextStage(stageResult);
    }
    window.dispatchEvent(
      new CustomEvent("game-next-stage", { detail: stageResult })
    );
  }, [nextStageUnlocked, stageResult]);

  const handleQuit = useCallback(() => {
    setIsResultOpen(false);
    setIsLeaderboardOpen(false);
    if (typeof window.onQuitToMenu === "function") {
      window.onQuitToMenu(stageResult);
    }
    window.dispatchEvent(
      new CustomEvent("game-quit", { detail: stageResult })
    );
  }, [stageResult]);

  const handleViewLeaderboard = useCallback(() => {
    setIsLeaderboardOpen(true);
    if (typeof window.onViewLeaderboard === "function") {
      window.onViewLeaderboard(stageResult);
    }
    window.dispatchEvent(
      new CustomEvent("view-leaderboard", { detail: stageResult })
    );
  }, [stageResult]);

  // Expose global methods for external modules and testing
  useEffect(() => {
    window.renderStageResult = updateStageResult;
    window.updateStageResult = updateStageResult;
    window.renderLeaderboard = renderLeaderboard;
    window.setLeaderboardTab = handleTabChange;
    window.getStageResultState = () => ({
      stageResult,
      isResultOpen,
      isLeaderboardOpen,
      isCleared,
      nextStageUnlocked,
      isFailed,
      isAllCleared,
    });

    return () => {
      delete window.renderStageResult;
      delete window.updateStageResult;
      delete window.renderLeaderboard;
      delete window.setLeaderboardTab;
      delete window.getStageResultState;
    };
  }, [updateStageResult, renderLeaderboard, handleTabChange, stageResult, isResultOpen, isLeaderboardOpen, isCleared, nextStageUnlocked, isFailed, isAllCleared]);

  return (
    <main className="relative min-h-screen w-full bg-surface-canvas font-sans text-content-primary">
      {/* =========================================================================
          MAIN MENU SCREEN (Campaign Map & Stage Select)
          Visible when user quits to menu or behind modal backdrop
          ========================================================================= */}
      <div id="main-menu" className="mx-auto flex min-h-screen max-w-[1200px] flex-col p-6 sm:p-10 gap-8">
        {/* Top Navbar */}
        <header className="flex flex-wrap items-center justify-between gap-4 border-b-2 border-stroke-default pb-6">
          <div className="flex items-center gap-3">
            <Badge
              variant="pink"
              label="WORD SPRINT"
              showIcon={false}
              className="border-2 border-stroke-default shadow-elevation-100 text-content-inverse px-[12px] py-[4px] text-xs font-mono font-bold"
            />
            <span className="m-0 font-sans text-[28px] font-bold tracking-tight text-content-primary uppercase">
              Campaign Map
            </span>
          </div>

          <div className="flex items-center gap-3">
            <Button
              id="btn-menu-leaderboard"
              label="Hall of Fame"
              variant="secondary"
              size="default"
              icon={<TrophyIcon className="size-[16px] [&_path]:fill-current" />}
              onClick={handleViewLeaderboard}
              className="shadow-elevation-200"
            />
            <Button
              id="btn-reopen-result"
              label="Stage Result"
              variant="primary"
              size="default"
              icon={<ArrowForwardIcon className="size-[16px] [&_path]:fill-current" />}
              onClick={() => setIsResultOpen(true)}
              className="shadow-elevation-200"
            />
          </div>
        </header>

        {/* Overview Header Cards: ProfileCard & ProgressSummaryCard */}
        <section className="flex flex-col md:flex-row items-stretch justify-between gap-6" aria-label="Campaign Overview">
          <ProfileCard
            label="CURRENT PLAYER"
            username={currentUserId}
            actionLabel="View Records"
            onSwitchProfile={handleViewLeaderboard}
            className="flex-1 shadow-elevation-200"
          />
          <ProgressSummaryCard
            status={isAllCleared ? "completed" : "available"}
            badgeLabel={isAllCleared ? "COMPLETED" : "IN PROGRESS"}
            eyebrow="OVERALL PROGRESS"
            title={isAllCleared ? "All Stages Conquered!" : "Campaign Active"}
            metaLabel="Campaign Score"
            metaValue={stageResult.campaignScore ?? 1450}
            onClick={handleViewLeaderboard}
            className="flex-[2] shadow-elevation-200"
          />
        </section>

        {/* Stage Selection Cards Grid */}
        <section className="flex flex-col gap-4" aria-label="Stage Selection">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-2">
              <span className="font-mono text-sm font-bold uppercase tracking-wider text-content-muted">
                SELECT STAGE
              </span>
              <span className="font-sans text-xs font-semibold text-content-secondary">
                (Click Play/Replay to open Stage Result)
              </span>
            </div>
            <span className="font-mono text-xs font-bold text-content-primary">
              {stageResult.isAllCleared ? "5/5 Cleared" : `${stageResult.stageId}/5 Progress`}
            </span>
          </div>

          <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-5 gap-6 justify-items-center">
            {DEFAULT_CAMPAIGN_STAGES.map((stage) => (
              <StageCard
                key={stage.id}
                stageId={stage.stageId}
                status={stage.status}
                topic={stage.topic}
                level={stage.level}
                imageSrc={heroImg}
                score={stage.score}
                onAction={() => {
                  setStageResult((prev) => ({
                    ...prev,
                    stageId: stage.id,
                    stageName: `${stage.stageId}: ${stage.topic.toUpperCase()}`,
                    score: stage.score,
                    solved: stage.id === 5 && prev.isAllCleared ? 8 : (stage.id < 5 ? 8 : prev.solved),
                  }));
                  setIsResultOpen(true);
                }}
                className="w-full max-w-none shadow-elevation-200"
              />
            ))}
          </div>
        </section>
      </div>

      {/* 1. STAGE RESULT / END GAME MODAL */}
      <Modal
        isOpen={isResultOpen && !isLeaderboardOpen}
        onClose={handleQuit}
        maxWidth="max-w-[740px]"
        className="w-[740px] max-w-full max-h-[92vh] border-2 border-stroke-default p-[24px] sm:p-[32px] shadow-elevation-200 flex flex-col justify-between items-center gap-[16px] bg-surface-panel"
        overlayClassName="bg-black/50 backdrop-blur-[20px]"
        closeOnBackdropClick={false}
        closeOnEscape={true}
      >
        {/* Header with Status Badge & Title */}
        <header className="flex w-full flex-col items-center justify-center gap-[12px] pt-[4px]">
          {isAllCleared ? (
            <Badge
              id="badge-status-top"
              variant="green"
              label="ALL STAGES CLEARED"
              showIcon={false}
              className="border-2 border-stroke-default shadow-elevation-100 text-content-primary px-[16px] py-[6px] text-xs font-mono font-bold tracking-wider"
            />
          ) : isFailed ? (
            <Badge
              id="badge-status-top"
              variant="red"
              label="TIME'S UP"
              showIcon={false}
              className="border-2 border-stroke-default shadow-elevation-100 text-content-inverse px-[16px] py-[6px] text-xs font-mono font-bold tracking-wider"
            />
          ) : (
            <Badge
              id="badge-status-top"
              variant="green"
              label="STAGE CLEARED"
              showIcon={false}
              className="border-2 border-stroke-default shadow-elevation-100 text-content-primary px-[16px] py-[6px] text-xs font-mono font-bold tracking-wider"
            />
          )}

          <ModalTitle
            as="h1"
            className="text-center font-sans font-bold uppercase tracking-tight text-content-primary text-[36px] leading-[40px] m-0"
          >
            {stageResult.stageName || `STAGE ${stageResult.stageId}: COMPLETED`}
          </ModalTitle>

          {/* Optional Rank Badge if provided */}
          {stageResult.rank && (
            <Badge
              id="badge-rank"
              variant="pink"
              label={`Rank #${stageResult.rank}`}
              showIcon={false}
              className="border-2 border-stroke-default shadow-elevation-100 text-content-inverse px-[14px] py-[4px] text-xs font-mono font-bold tracking-wider"
            />
          )}
        </header>

        {/* 1. Stage Statistics InfoPill Row */}
        <section
          className="flex w-full max-w-[640px] items-center justify-between gap-[16px]"
          aria-label="Stage Statistics"
        >
          <InfoPill
            id="pill-quiz-solved"
            label="Quiz Solved"
            value={`${stageResult.solved ?? 0}/${stageResult.totalQuiz ?? 0}`}
            showLeadingIcon={false}
            showTrailingIcon={false}
            className="flex-1 justify-center shadow-elevation-200"
          />
          <InfoPill
            id="pill-play-time"
            label="Play time"
            value={formatPlayTime(stageResult.playTime)}
            showLeadingIcon={false}
            showTrailingIcon={false}
            className="flex-1 justify-center shadow-elevation-200"
          />
          <InfoPill
            id="pill-score"
            label="Score"
            value={stageResult.score ?? 0}
            showLeadingIcon={false}
            showTrailingIcon={false}
            className="flex-1 justify-center shadow-elevation-200"
          />
        </section>

        {/* 2. Middle Content Area: Condition Note OR Overall Progress Card */}
        {isAllCleared ? (
          /* Frame 2: OVERALL PROGRESS Card */
          <section
            id="card-overall-progress"
            className="flex w-full max-w-[640px] flex-col gap-[16px] border-2 border-stroke-default bg-surface-panel p-[20px] shadow-elevation-200 box-border"
            aria-label="Overall Progress"
          >
            <div className="flex items-center justify-between">
              <h2 className="font-sans font-bold text-heading-lg text-content-primary tracking-tight">
                All Stages Conquered!
              </h2>
              <Badge
                id="badge-completed"
                variant="green"
                label="COMPLETED"
                showIcon={true}
                icon={<CheckIcon className="size-[12px] [&_path]:fill-current" />}
                className="border-2 border-stroke-default shadow-elevation-100 text-content-primary px-[10px] py-[3px] text-xs font-mono font-bold"
              />
            </div>

            {/* Campaign Score Stat Box */}
            <div className="flex w-full items-center justify-between border-2 border-stroke-default bg-surface-primary-subtle p-[12px] px-[16px] shadow-elevation-100 box-border">
              <span className="font-sans text-body-default font-bold text-content-primary">
                Campaign Score
              </span>
              <span id="text-campaign-score" className="font-sans text-heading-lg font-bold text-content-primary">
                {stageResult.campaignScore ?? stageResult.score}
              </span>
            </div>
          </section>
        ) : isFailed ? (
          /* Frame 1: Unlock Condition Note */
          <section
            id="note-unlock-condition"
            className="flex w-full max-w-[640px] items-center justify-center border-2 border-stroke-default bg-surface-primary-subtle p-[16px] shadow-elevation-100 box-border text-center"
            aria-label="Unlock Condition"
          >
            <span className="font-sans text-body-default font-medium text-content-primary">
              Solve at least {stageResult.unlockThreshold ?? 4} quizzes to unlock new stage
            </span>
          </section>
        ) : (
          /* Regular Stage Cleared Notice */
          <section
            id="note-cleared"
            className="flex w-full max-w-[640px] items-center justify-center border-2 border-stroke-default bg-surface-success-subtle p-[16px] shadow-elevation-100 box-border text-center"
            aria-label="Stage Success"
          >
            <span className="font-sans text-body-default font-bold text-content-success">
              Stage Cleared! You unlocked the next stage.
            </span>
          </section>
        )}

        {/* 3. Action Navigation Buttons */}
        <footer
          className="flex w-full max-w-[640px] flex-col gap-[16px] pb-[4px]"
          aria-label="Modal Actions"
        >
          {isAllCleared ? (
            /* Frame 2 Actions: VIEW LEADERBOARD + (REPLAY | QUIT) */
            <>
              <Button
                id="btn-view-leaderboard"
                label="VIEW LEADERBOARD"
                variant="primary"
                size="large"
                leadingIcon={<TrophyIcon className="size-[20px] [&_path]:fill-current" />}
                trailingIcon={<ArrowForwardIcon className="size-[16px] [&_path]:fill-current" />}
                showLeadingIcon={true}
                showTrailingIcon={true}
                onClick={handleViewLeaderboard}
                className="w-full h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-primary-cta"
              />
              <div className="flex w-full items-center justify-between gap-[24px]">
                <Button
                  id="btn-replay-game"
                  label="REPLAY"
                  variant="secondary"
                  size="large"
                  leadingIcon={<ReplayIcon className="size-[20px] [&_path]:fill-current" />}
                  showTrailingIcon={false}
                  onClick={handleTryAgainOrReplay}
                  className="flex-1 h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-panel"
                />
                <Button
                  id="btn-quit-menu"
                  label="QUIT TO MENU"
                  variant="secondary"
                  size="large"
                  leadingIcon={<ExitIcon className="size-[20px] [&_path]:fill-current" />}
                  showTrailingIcon={false}
                  onClick={handleQuit}
                  className="flex-1 h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-panel"
                />
              </div>
            </>
          ) : isFailed ? (
            /* Frame 1 Actions: TRY AGAIN + (QUIT | NEXT STAGE (disabled)) */
            <>
              <Button
                id="btn-try-again"
                label="TRY AGAIN"
                variant="primary"
                size="large"
                leadingIcon={<ReplayIcon className="size-[20px] [&_path]:fill-current" />}
                showTrailingIcon={false}
                onClick={handleTryAgainOrReplay}
                className="w-full h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-primary-cta"
              />
              <div className="flex w-full items-center justify-between gap-[24px]">
                <Button
                  id="btn-quit-menu"
                  label="QUIT TO MENU"
                  variant="secondary"
                  size="large"
                  leadingIcon={<ExitIcon className="size-[20px] [&_path]:fill-current" />}
                  showTrailingIcon={false}
                  onClick={handleQuit}
                  className="flex-1 h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-panel"
                />
                <Button
                  id="btn-next-stage"
                  label="NEXT STAGE"
                  variant="disabled"
                  disabled={true}
                  size="large"
                  trailingIcon={<ArrowForwardIcon className="size-[16px] [&_path]:fill-content-muted" />}
                  showTrailingIcon={true}
                  showLeadingIcon={false}
                  className="flex-1 h-[52px] border-2 border-stroke-disabled shadow-elevation-0 text-button-lg font-bold bg-surface-disabled text-content-muted cursor-not-allowed opacity-60"
                />
              </div>
            </>
          ) : (
            /* Stage Cleared: NEXT STAGE (active) + (REPLAY | QUIT) */
            <>
              <Button
                id="btn-next-stage"
                label="NEXT STAGE"
                variant="primary"
                size="large"
                trailingIcon={<ArrowForwardIcon className="size-[16px] [&_path]:fill-current" />}
                showTrailingIcon={true}
                showLeadingIcon={false}
                onClick={handleNextStage}
                className="w-full h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-primary-cta"
              />
              <div className="flex w-full items-center justify-between gap-[24px]">
                <Button
                  id="btn-replay-game"
                  label="REPLAY"
                  variant="secondary"
                  size="large"
                  leadingIcon={<ReplayIcon className="size-[20px] [&_path]:fill-current" />}
                  showTrailingIcon={false}
                  onClick={handleTryAgainOrReplay}
                  className="flex-1 h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-panel"
                />
                <Button
                  id="btn-quit-menu"
                  label="QUIT TO MENU"
                  variant="secondary"
                  size="large"
                  leadingIcon={<ExitIcon className="size-[20px] [&_path]:fill-current" />}
                  showTrailingIcon={false}
                  onClick={handleQuit}
                  className="flex-1 h-[52px] border-2 border-stroke-default shadow-elevation-200 text-button-lg font-bold bg-surface-panel"
                />
              </div>
            </>
          )}
        </footer>
      </Modal>

      {/* 2. HALL OF FAME LEADERBOARD MODAL (Opened from VIEW LEADERBOARD) */}
      <Modal
        isOpen={isLeaderboardOpen}
        onClose={() => setIsLeaderboardOpen(false)}
        maxWidth="max-w-[752px]"
        className="w-[752px] min-h-[auto] max-h-[92vh] border-2 border-stroke-default p-8 sm:p-12 shadow-elevation-200 flex flex-col items-center gap-8 bg-surface-panel"
        overlayClassName="bg-black/50 backdrop-blur-[20px]"
        closeOnBackdropClick={true}
        closeOnEscape={true}
      >
        {/* Header with Title */}
        <header className="flex w-full items-center justify-center">
          <ModalTitle as="h1" className="text-center font-sans text-4xl font-bold uppercase tracking-tight text-content-primary">
            HALL OF FAME
          </ModalTitle>
        </header>

        {/* Navigation Tabs Bar */}
        <section className="flex w-full justify-center">
          <Navigation
            items={TAB_ITEMS}
            activeTab={activeTab}
            onChange={handleTabChange}
            className="w-[652px] max-w-full border-2 border-stroke-default p-1.5 shadow-elevation-200"
          />
        </section>

        {/* Leaderboard Table Container */}
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
            <div id="leaderboard-table-body" className="max-h-[336px] overflow-y-auto flex flex-col divide-y divide-stroke-default">
              {records.length === 0 ? (
                <div className="flex h-36 items-center justify-center font-sans text-base font-bold text-content-muted uppercase tracking-wider">
                  Chưa có dữ liệu
                </div>
            )}

            {/* ========================================================================= */}
            {/* 1. Modal "SELECT A PROFILE" (Figma Node 2282:2090)                         */}
            {/* ========================================================================= */}
            <Modal
                isOpen={isSelectProfileOpen}
                onClose={() => setIsSelectProfileOpen(false)}
                maxWidth="max-w-[560px]"
                className="w-[560px] min-h-[auto] max-h-[92vh] border-2 border-stroke-default p-lg shadow-elevation-200 flex flex-col items-center gap-lg bg-surface-panel"
                overlayClassName="bg-black/50 backdrop-blur-[20px]"
                closeOnBackdropClick={true}
                closeOnEscape={true}
            >
                {/* Header */}
                <header className="flex w-full items-center justify-center">
                    <ModalTitle
                        as="h1"
                        className="text-center font-sans text-heading-lg font-bold uppercase tracking-tight text-content-primary"
                    >
                        SELECT A PROFILE
                    </ModalTitle>
                </header>

                {/* Profiles Table Container */}
                <section className="flex w-full justify-center">
                    <div className="w-full border-2 border-stroke-default bg-surface-panel flex flex-col shadow-elevation-200">
                        {/* Header Row */}
                        <ProfileRecord
                            variant="header"
                            playerName="PLAYER"
                            progress="PROGRESS"
                        />

                        {/* Dynamic Profiles Rows Body (rendered from state, not hardcoded) */}
                        <div
                            id="profile-table-body"
                            className="max-h-[300px] overflow-y-auto flex flex-col divide-y-2 divide-stroke-default border-t-2 border-stroke-default"
                        >
                            {profiles.length === 0 ? (
                                <div className="flex h-28 items-center justify-center font-sans text-sm font-bold text-content-muted uppercase tracking-wider">
                                    Chưa có hồ sơ người chơi
                                </div>
                            ) : (
                                profiles.map((profile) => {
                                    const isYou =
                                        String(profile.id) ===
                                            String(activeProfileId) ||
                                        profile.username === activeProfileId ||
                                        (activeProfile &&
                                            profile.username ===
                                                activeProfile.username);

                                    return (
                                        <ProfileRecord
                                            key={profile.id}
                                            variant={isYou ? "you" : "default"}
                                            playerName={profile.username}
                                            progress={
                                                profile.progress || "0/12"
                                            }
                                            badgeLabel="YOU"
                                            onSelect={() =>
                                                handleSelectProfile(profile.id)
                                            }
                                            onDelete={(e) =>
                                                handleDeleteProfile(
                                                    profile.id,
                                                    e
                                                )
                                            }
                                        />
                                    );
                                })
                            )}
                        </div>
                    </div>
                </section>

                {/* Footer Buttons: "✕ Close" and "+ Add" */}
                <footer className="flex w-full items-center justify-between gap-md pt-xs">
                    <Button
                        label="Close"
                        variant="secondary"
                        size="default"
                        leadingIcon={<CloseIcon className="size-icon-sm" />}
                        showTrailingIcon={false}
                        onClick={() => setIsSelectProfileOpen(false)}
                        className="min-w-[130px] border-2 border-stroke-default shadow-elevation-200 text-button-default font-bold font-sans"
                    />
                  );
                })
              )}
            </div>
          </div>
        </section>

        {/* Footer with Close Button */}
        <footer className="flex w-full items-center justify-center pt-2">
          <Button
            id="btn-close-leaderboard"
            label="Close"
            variant="secondary"
            size="default"
            leadingIcon={<CloseIcon className="size-3.5" />}
            showTrailingIcon={false}
            onClick={() => setIsLeaderboardOpen(false)}
            className="w-[120px] h-[40px] border-2 border-stroke-default shadow-elevation-200 text-sm font-bold bg-surface-panel"
          />
        </footer>
      </Modal>
    </main>
  );
}

export default App;
