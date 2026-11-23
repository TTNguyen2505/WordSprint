import { useId, useState } from "react";
import CloseIcon from "../assets/icons/close.svg?react";
import Button from "../components/Button";
import LeaderboardRecord from "../components/LeaderboardRecord";
import Modal from "../components/Modal";
import ModalTitle from "../components/ModalTitle";
import Navigation from "../components/Navigation";

const mockLeaderboards = {
    Overall: [
        { rank: 1, player: "nguyen", score: 100000, playTime: "10:00" },
        {
            rank: 2,
            player: "nguyen_02",
            score: 150000,
            playTime: "10:00",
            isCurrentPlayer: true
        },
        { rank: 3, player: "nguyen_03", score: 120000, playTime: "10:00" },
        ...Array.from({ length: 7 }, (_, index) => ({
            rank: index + 4,
            player: "nguyen_04",
            score: 120000,
            playTime: "10:00"
        }))
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

function LeaderboardModal({ isOpen = true, onClose }) {
    const titleId = useId();
    const [activeTab, setActiveTab] = useState("Overall");
    const records = mockLeaderboards[activeTab];

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
                    <Navigation activeTab={activeTab} onChange={setActiveTab} />
                    <div
                        role="table"
                        aria-label={`${activeTab} leaderboard`}
                        className="h-[384px] w-max overflow-y-auto"
                    >
                        <LeaderboardRecord
                            type="Header"
                            className="sticky top-0 z-10 h-auto w-max min-w-0"
                        />
                        {records.map((record) => (
                            <LeaderboardRecord
                                key={record.rank}
                                type={
                                    record.isCurrentPlayer
                                        ? "Active"
                                        : record.rank === 1
                                          ? "First Record"
                                          : "Default"
                                }
                                rank={record.rank}
                                playerName={record.player}
                                score={record.score}
                                playTime={record.playTime}
                                className="h-auto w-max min-w-0"
                            />
                        ))}
                        {records.length === 0 && (
                            <div role="row">
                                <div
                                    role="cell"
                                    className="text-body-default p-md text-center text-content-muted"
                                >
                                    No records yet.
                                </div>
                            </div>
                        )}
                    </div>
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
