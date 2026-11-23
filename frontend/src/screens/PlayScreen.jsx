import { useEffect, useState } from "react";
import ClockIcon from "../assets/icons/clock.svg?react";
import PauseIcon from "../assets/icons/pause.svg?react";
import TopicIcon from "../assets/icons/topic.svg?react";
import Badge from "../components/Badge";
import Button from "../components/Button";
import InfoPill from "../components/InfoPill";
import PipelineItem from "../components/PipelineItem";
import Quiz from "../components/Quiz";
import PauseModal from "./PauseModal";

const questionCount = 8;

function PlayScreen({
    username = "Player_1",
    score = 0,
    topic = "SCHOOL",
    level = "A1",
    time = "10:00",
    onPreviewResult,
    onReplay,
    onQuit
}) {
    const [activeQuestion, setActiveQuestion] = useState(0);
    const [answers, setAnswers] = useState(() => Array(questionCount).fill(""));
    const [paused, setPaused] = useState(false);
    const [message, setMessage] = useState("");
    const [countdown, setCountdown] = useState(3);

    useEffect(() => {
        if (countdown === 0) return undefined;

        const timeoutId = window.setTimeout(() => {
            setCountdown((current) => current - 1);
        }, 1000);
        return () => window.clearTimeout(timeoutId);
    }, [countdown]);

    const selectQuestion = (index) => {
        setActiveQuestion(index);
        setMessage("");
    };

    const updateAnswer = (answer) => {
        setAnswers((current) =>
            current.map((value, index) =>
                index === activeQuestion ? answer : value
            )
        );
        setMessage("");
    };

    const resume = () => {
        setPaused(false);
        setCountdown(3);
    };

    return (
        <>
            {countdown > 0 ? (
                <main className="flex h-[900px] w-[1200px] items-center justify-center bg-surface-canvas text-content-primary">
                    <p
                        role="status"
                        aria-atomic="true"
                        className="text-display-hero"
                    >
                        <span className="sr-only">Starting in </span>
                        {countdown}
                    </p>
                </main>
            ) : (
                <div inert={paused}>
                    <main className="flex h-[900px] w-[1200px] bg-surface-canvas text-content-primary">
                        <aside className="flex h-full w-[360px] shrink-0 flex-col bg-surface-panel">
                            <header className="border-[length:var(--border-width-heavy)] border-stroke-default px-md py-sm">
                                <Button
                                    label="PAUSE"
                                    size="default"
                                    variant="secondary"
                                    className="w-full"
                                    leadingIcon={
                                        <PauseIcon className="size-full" />
                                    }
                                    showTrailingIcon={false}
                                    onClick={() => setPaused(true)}
                                />
                            </header>
                            <section
                                aria-label="Questions"
                                className="flex min-h-0 flex-1 flex-col gap-lg overflow-y-auto border-x-[length:var(--border-width-heavy)] border-b-[length:var(--border-width-heavy)] border-stroke-default px-md pt-2xl pb-sm"
                            >
                                <h2 className="text-heading-sm">
                                    QUESTIONS (0/{questionCount})
                                </h2>
                                <div className="flex flex-col gap-md">
                                    {Array.from(
                                        { length: questionCount },
                                        (_, index) => (
                                            <PipelineItem
                                                key={index}
                                                questionOrdinary={`Question ${index + 1}`}
                                                status={
                                                    index === activeQuestion
                                                        ? "Active"
                                                        : "Queued"
                                                }
                                                className="w-full"
                                                onClick={
                                                    paused
                                                        ? undefined
                                                        : () =>
                                                              selectQuestion(
                                                                  index
                                                              )
                                                }
                                            />
                                        )
                                    )}
                                </div>
                            </section>
                            <footer className="flex flex-col gap-sm border-x-[length:var(--border-width-heavy)] border-b-[length:var(--border-width-heavy)] border-stroke-default px-md py-sm">
                                <div className="flex items-center justify-between gap-xs">
                                    <span className="text-heading-sm">
                                        CURRENT PLAYER:
                                    </span>
                                    <Badge type="Highlight" showIcon={false}>
                                        {username}
                                    </Badge>
                                </div>
                                <div className="border-t-[length:var(--border-width-default)] border-stroke-default" />
                                <div className="flex items-center justify-between gap-xs">
                                    <span className="text-heading-sm">
                                        ACCUMULATED SCORE:
                                    </span>
                                    <Badge type="Highlight" showIcon={false}>
                                        {score}
                                    </Badge>
                                </div>
                            </footer>
                        </aside>

                        <section
                            aria-label="Quiz"
                            className="flex h-full w-[840px] flex-col items-center justify-between overflow-hidden px-2xl pt-xl"
                        >
                            <header className="flex w-full items-center justify-between">
                                <div className="flex items-center gap-[10px]">
                                    <InfoPill
                                        label="Topic"
                                        value={topic}
                                        leadingIcon={
                                            <TopicIcon className="size-full" />
                                        }
                                        showTrailingIcon={false}
                                    />
                                    <InfoPill
                                        label="Level"
                                        value={level}
                                        showLeadingIcon={false}
                                        showTrailingIcon={false}
                                    />
                                </div>
                                <InfoPill
                                    label="Time"
                                    value={time}
                                    leadingIcon={
                                        <ClockIcon className="size-full" />
                                    }
                                    showTrailingIcon={false}
                                />
                            </header>
                            <div className="relative w-full">
                                <Quiz
                                    className="max-w-none"
                                    answer={answers[activeQuestion]}
                                    onChange={updateAnswer}
                                    onSubmit={() =>
                                        setMessage(
                                            "Answer checking is unavailable in this preview."
                                        )
                                    }
                                    onPrevious={() =>
                                        selectQuestion(activeQuestion - 1)
                                    }
                                    onNext={() =>
                                        selectQuestion(activeQuestion + 1)
                                    }
                                    hasPrevious={activeQuestion > 0}
                                    hasNext={activeQuestion < questionCount - 1}
                                    disabled={paused}
                                />
                                {message && (
                                    <p
                                        role="status"
                                        className="text-body-default absolute top-full mt-xs w-full text-center"
                                    >
                                        {message}
                                    </p>
                                )}
                            </div>
                            {onPreviewResult ? (
                                <div className="flex items-center gap-sm pb-lg">
                                    <span className="text-body-default">
                                        Mock results:
                                    </span>
                                    <Button
                                        label="Stage cleared"
                                        size="small"
                                        variant="secondary"
                                        showLeadingIcon={false}
                                        showTrailingIcon={false}
                                        disabled={paused}
                                        onClick={() =>
                                            onPreviewResult("cleared")
                                        }
                                    />
                                    <Button
                                        label="Time's up"
                                        size="small"
                                        variant="secondary"
                                        showLeadingIcon={false}
                                        showTrailingIcon={false}
                                        disabled={paused}
                                        onClick={() =>
                                            onPreviewResult("timeUp")
                                        }
                                    />
                                </div>
                            ) : (
                                <div />
                            )}
                        </section>
                    </main>
                </div>
            )}
            <PauseModal
                isOpen={paused && countdown === 0}
                solved={0}
                total={questionCount}
                timeLeft={time}
                score={score}
                onResume={resume}
                onReplay={onReplay}
                onQuit={onQuit}
            />
        </>
    );
}

export default PlayScreen;
