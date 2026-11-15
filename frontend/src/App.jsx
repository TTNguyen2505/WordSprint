import { useState } from "react";
import heroImg from "./assets/hero.png";
import reactLogo from "./assets/react.svg";
import viteLogo from "./assets/vite.svg";

function CornerDivider() {
    return (
        <div
            aria-hidden="true"
            className="relative h-0 w-full border-t border-ui-border before:absolute before:-top-[5px] before:left-0 before:box-content before:border-[5px] before:border-solid before:border-transparent before:border-l-ui-border before:content-[''] after:absolute after:-top-[5px] after:right-0 after:box-content after:border-[5px] after:border-solid after:border-transparent after:border-r-ui-border after:content-['']"
        />
    );
}

function SocialLink({ href, children }) {
    return (
        <a
            href={href}
            target="_blank"
            rel="noreferrer"
            className="flex items-center justify-start gap-[0.5rem] rounded-[0.375rem] bg-ui-social px-[0.75rem] py-[0.375rem] text-subtitle text-ui-heading no-underline transition-shadow hover:shadow-social max-lg:w-full max-lg:justify-center"
        >
            {children}
        </a>
    );
}

function App() {
    const [count, setCount] = useState(0);

    return (
        <main className="mx-auto box-border flex min-h-[100svh] w-full max-w-[1126px] flex-col border-x border-ui-border font-sans text-body text-center text-ui-text">
            <section className="flex grow flex-col items-center justify-center gap-[1.5625rem] max-lg:gap-[1.125rem] max-lg:px-[1.25rem] max-lg:pt-xl max-lg:pb-sm">
                <div className="relative">
                    <img
                        src={heroImg}
                        className="relative z-0 mx-auto w-[170px]"
                        width="170"
                        height="179"
                        alt=""
                    />
                    <img
                        src={reactLogo}
                        className="absolute inset-x-0 top-[34px] z-[1] mx-auto h-[28px] transform-[perspective(2000px)_rotateZ(300deg)_rotateX(44deg)_rotateY(39deg)_scale(1.4)]"
                        alt="React logo"
                    />
                    <img
                        src={viteLogo}
                        className="absolute inset-x-0 top-[107px] z-0 mx-auto h-[26px] w-auto transform-[perspective(2000px)_rotateZ(300deg)_rotateX(40deg)_rotateY(39deg)_scale(0.8)]"
                        alt="Vite logo"
                    />
                </div>
                <div>
                    <h1 className="my-xl text-heading-xl text-ui-heading max-lg:my-[1.25rem] max-lg:text-heading-lg">
                        Get started
                    </h1>
                    <p className="text-subtitle">
                        Edit{" "}
                        <code className="inline-flex rounded-[0.25rem] bg-ui-code px-[0.5rem] py-[0.25rem] font-mono text-mono-data text-ui-heading">
                            src/App.jsx
                        </code>{" "}
                        and save to test{" "}
                        <code className="inline-flex rounded-[0.25rem] bg-ui-code px-[0.5rem] py-[0.25rem] font-mono text-mono-data text-ui-heading">
                            HMR
                        </code>
                    </p>
                </div>
                <button
                    type="button"
                    className="mb-sm inline-flex items-center rounded-[0.3125rem] border-2 border-transparent bg-ui-accent-bg px-[0.625rem] py-[0.3125rem] text-btn text-ui-accent transition-[border-color] hover:border-ui-accent-border focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-ui-accent"
                    onClick={() => setCount((currentCount) => currentCount + 1)}
                >
                    Count is {count}
                </button>
            </section>

            <CornerDivider />

            <section className="flex border-t border-ui-border text-left max-lg:flex-col max-lg:text-center">
                <div className="flex-1 border-r border-ui-border p-xl max-lg:border-r-0 max-lg:border-b max-lg:p-sm max-lg:px-[1.25rem]">
                    <svg
                        className="mb-md h-[22px] w-[22px]"
                        role="presentation"
                        aria-hidden="true"
                    >
                        <use href="./icons.svg#documentation-icon"></use>
                    </svg>
                    <h2 className="mb-[0.5rem] text-heading-lg text-ui-heading max-lg:text-heading-md">
                        Documentation
                    </h2>
                    <p className="text-subtitle">Your questions, answered</p>
                    <ul className="mt-xl flex list-none gap-[0.5rem] p-0 max-lg:mt-[1.25rem] max-lg:flex-wrap max-lg:justify-center">
                        <li className="flex-[0_1_auto] max-lg:flex-[1_1_calc(50%_-_0.5rem)]">
                            <SocialLink href="https://vite.dev/">
                                <img
                                    className="h-[18px]"
                                    src={viteLogo}
                                    alt=""
                                />
                                Explore Vite
                            </SocialLink>
                        </li>
                        <li className="flex-[0_1_auto] max-lg:flex-[1_1_calc(50%_-_0.5rem)]">
                            <SocialLink href="https://react.dev/">
                                <img
                                    className="h-[18px] w-[18px]"
                                    src={reactLogo}
                                    alt=""
                                />
                                Learn more
                            </SocialLink>
                        </li>
                    </ul>
                </div>
                <div className="flex-1 p-xl max-lg:p-sm max-lg:px-[1.25rem]">
                    <svg
                        className="mb-md h-[22px] w-[22px]"
                        role="presentation"
                        aria-hidden="true"
                    >
                        <use href="./icons.svg#social-icon"></use>
                    </svg>
                    <h2 className="mb-[0.5rem] text-heading-lg text-ui-heading max-lg:text-heading-md">
                        Connect with us
                    </h2>
                    <p className="text-subtitle">Join the Vite community</p>
                    <ul className="mt-xl flex list-none gap-[0.5rem] p-0 max-lg:mt-[1.25rem] max-lg:flex-wrap max-lg:justify-center">
                        <li className="flex-[0_1_auto] max-lg:flex-[1_1_calc(50%_-_0.5rem)]">
                            <SocialLink href="https://github.com/vitejs/vite">
                                <svg
                                    className="h-[18px] w-[18px] dark:[filter:invert(1)_brightness(2)]"
                                    role="presentation"
                                    aria-hidden="true"
                                >
                                    <use href="./icons.svg#github-icon"></use>
                                </svg>
                                GitHub
                            </SocialLink>
                        </li>
                        <li className="flex-[0_1_auto] max-lg:flex-[1_1_calc(50%_-_0.5rem)]">
                            <SocialLink href="https://chat.vite.dev/">
                                <svg
                                    className="h-[18px] w-[18px] dark:[filter:invert(1)_brightness(2)]"
                                    role="presentation"
                                    aria-hidden="true"
                                >
                                    <use href="./icons.svg#discord-icon"></use>
                                </svg>
                                Discord
                            </SocialLink>
                        </li>
                        <li className="flex-[0_1_auto] max-lg:flex-[1_1_calc(50%_-_0.5rem)]">
                            <SocialLink href="https://x.com/vite_js">
                                <svg
                                    className="h-[18px] w-[18px] dark:[filter:invert(1)_brightness(2)]"
                                    role="presentation"
                                    aria-hidden="true"
                                >
                                    <use href="./icons.svg#x-icon"></use>
                                </svg>
                                X.com
                            </SocialLink>
                        </li>
                        <li className="flex-[0_1_auto] max-lg:flex-[1_1_calc(50%_-_0.5rem)]">
                            <SocialLink href="https://bsky.app/profile/vite.dev">
                                <svg
                                    className="h-[18px] w-[18px] dark:[filter:invert(1)_brightness(2)]"
                                    role="presentation"
                                    aria-hidden="true"
                                >
                                    <use href="./icons.svg#bluesky-icon"></use>
                                </svg>
                                Bluesky
                            </SocialLink>
                        </li>
                    </ul>
                </div>
            </section>

            <CornerDivider />
            <section className="h-[88px] border-t border-ui-border max-lg:h-[48px]" />
        </main>
    );
}

export default App;
