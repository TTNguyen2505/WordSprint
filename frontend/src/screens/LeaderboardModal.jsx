import { useEffect, useId, useRef } from "react";
import ArrowBackIcon from "../assets/icons/arrow_back_1.svg?react";
import ArrowForwardIcon from "../assets/icons/arrow_forward_1.svg?react";
import CloseIcon from "../assets/icons/close.svg?react";
import Button from "../components/Button";
import IconButton from "../components/IconButton";
import LeaderboardRecord from "../components/LeaderboardRecord";
import Modal from "../components/Modal";
import ModalTitle from "../components/ModalTitle";
import Navigation from "../components/Navigation";
import { useLeaderboardViewModel } from "../viewModels/leaderboardViewModel";
import { useMenuViewModel } from "../viewModels/menuViewModel";

function formatTime(seconds) {
    if (seconds == null) return "-";
    return `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, "0")}`;
}

function LeaderboardModal({ isOpen = true, onClose }) {
    const titleId = useId();
    const tabsRef = useRef(null);
    const profile = useMenuViewModel();
    const activeTab = useLeaderboardViewModel((state) => state.activeTab);
    const records = useLeaderboardViewModel((state) => state.records);
    const status = useLeaderboardViewModel((state) => state.status);
    const error = useLeaderboardViewModel((state) => state.error);
    const selectTab = useLeaderboardViewModel((state) => state.selectTab);
    const stages = [...(profile?.stages ?? [])].sort((a, b) => a.id - b.id);
    const items = [
        { label: "Overall", value: "overall" },
        ...stages.map((stage) => ({ label: stage.stageId, value: stage.id }))
    ];
    const activeLabel = items.find((item) => item.value === activeTab)?.label;

    useEffect(() => {
        if (isOpen) selectTab("overall");
    }, [isOpen, selectTab]);

    const scrollTabs = (direction) => {
        tabsRef.current?.scrollBy({
            left: direction * 240,
            behavior: "smooth"
        });
    };

    return (
        <Modal
            isOpen={isOpen}
            onClose={onClose}
            aria-labelledby={titleId}
            header={
                <ModalTitle
                    id={titleId}
                    className="w-full text-center text-[length:var(--primitive-font-size-36)] leading-[var(--primitive-line-height-40)]"
                >
                    HALL OF FAME
                </ModalTitle>
            }
            body={
                <div className="flex w-[652px] flex-col items-center gap-xl">
                    <div className="flex w-full items-center gap-xs">
                        <IconButton
                            ariaLabel="Previous leaderboard tabs"
                            icon={<ArrowBackIcon className="size-full" />}
                            onClick={() => scrollTabs(-1)}
                        />
                        <Navigation
                            navigationRef={tabsRef}
                            items={items}
                            activeTab={activeTab}
                            onChange={selectTab}
                            className="min-w-0 flex-1"
                        />
                        <IconButton
                            ariaLabel="Next leaderboard tabs"
                            icon={<ArrowForwardIcon className="size-full" />}
                            onClick={() => scrollTabs(1)}
                        />
                    </div>
                    <div
                        role="table"
                        aria-label={`${activeLabel} leaderboard`}
                        className="h-[384px] w-max overflow-y-auto"
                    >
                        <LeaderboardRecord
                            type="Header"
                            className="sticky top-0 z-10 h-auto w-max min-w-0"
                        />
                        {status === "ready" &&
                            records.map((record) => (
                                <LeaderboardRecord
                                    key={record.id ?? record.userId}
                                    type={
                                        record.userId === profile?.id
                                            ? "Active"
                                            : record.rank === 1
                                              ? "First Record"
                                              : "Default"
                                    }
                                    rank={record.rank}
                                    playerName={record.player}
                                    score={record.score}
                                    playTime={formatTime(record.completionTime)}
                                    className="h-auto w-max min-w-0"
                                />
                            ))}
                        {status === "ready" && records.length === 0 && (
                            <p
                                role="status"
                                className="text-body-default p-md text-center text-content-muted"
                            >
                                No records yet.
                            </p>
                        )}
                        {status === "loading" && (
                            <p role="status" className="p-md text-center">
                                Loading leaderboard...
                            </p>
                        )}
                        {status === "error" && (
                            <div role="alert" className="p-md text-center">
                                <p>{error}</p>
                                <Button
                                    label="Retry"
                                    showLeadingIcon={false}
                                    showTrailingIcon={false}
                                    onClick={() => selectTab(activeTab)}
                                />
                            </div>
                        )}
                    </div>
                    <p className="text-body-sm text-content-muted">
                        Top 100 records. Overall scores do not include play
                        time.
                    </p>
                </div>
            }
            footer={
                <Button
                    label="Close"
                    size="default"
                    variant="secondary"
                    leadingIcon={<CloseIcon className="size-full" />}
                    showTrailingIcon={false}
                    onClick={onClose}
                />
            }
        />
    );
}

export default LeaderboardModal;
