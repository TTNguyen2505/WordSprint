import { useState, useEffect, useCallback, useMemo } from "react";
import Modal from "./components/Modal.jsx";
import ModalTitle from "./components/ModalTitle.jsx";
import ProfileRecord from "./components/ProfileRecord.jsx";
import ProfileCard from "./components/ProfileCard.jsx";
import TextInput from "./components/TextInput.jsx";
import Button from "./components/Button.jsx";
import CloseIcon from "./assets/icons/close.svg?react";
import AddIcon from "./assets/icons/add.svg?react";
import AccountIcon from "./assets/icons/account.svg?react";

const PROFILES_STORAGE_KEY = "wordsprint_profiles";
const ACTIVE_PROFILE_STORAGE_KEY = "wordsprint_active_profile_id";

// Initial illustrative dataset loaded dynamically via state/localStorage (never hardcoded in markup)
const DEFAULT_INITIAL_PROFILES = [
    { id: "1", username: "nguyen_02", progress: "7/12" },
    { id: "2", username: "nguyen_03", progress: "6/12" },
    { id: "3", username: "duy_bao", progress: "4/12" },
    { id: "4", username: "tuan_khang", progress: "2/12" }
];

function loadStoredProfiles() {
    try {
        const raw = localStorage.getItem(PROFILES_STORAGE_KEY);
        if (raw) {
            const parsed = JSON.parse(raw);
            if (Array.isArray(parsed) && parsed.length > 0) {
                return parsed;
            }
        }
    } catch (err) {
        console.error("Failed to parse stored profiles from localStorage", err);
    }
    return DEFAULT_INITIAL_PROFILES;
}

function loadStoredActiveId(profiles) {
    try {
        const stored = localStorage.getItem(ACTIVE_PROFILE_STORAGE_KEY);
        if (
            stored &&
            profiles.some(
                (p) => String(p.id) === String(stored) || p.username === stored
            )
        ) {
            return stored;
        }
    } catch (err) {
        console.error(
            "Failed to read active profile id from localStorage",
            err
        );
    }
    return profiles[0]?.id || "1";
}

function App() {
    // 1. Data States
    const [profiles, setProfiles] = useState(loadStoredProfiles);
    const [activeProfileId, setActiveProfileId] = useState(() =>
        loadStoredActiveId(profiles)
    );

    // 2. Modal Visibility States
    const [isSelectProfileOpen, setIsSelectProfileOpen] = useState(true);
    const [isCreateUserOpen, setIsCreateUserOpen] = useState(false);

    // 3. Create User Form State
    const [newUsername, setNewUsername] = useState("");
    const [createError, setCreateError] = useState("");

    // Sync to localStorage
    useEffect(() => {
        try {
            localStorage.setItem(
                PROFILES_STORAGE_KEY,
                JSON.stringify(profiles)
            );
        } catch (err) {
            console.error("Failed to save profiles to localStorage", err);
        }
    }, [profiles]);

    useEffect(() => {
        try {
            if (activeProfileId) {
                localStorage.setItem(
                    ACTIVE_PROFILE_STORAGE_KEY,
                    String(activeProfileId)
                );
            }
        } catch (err) {
            console.error(
                "Failed to save activeProfileId to localStorage",
                err
            );
        }
    }, [activeProfileId]);

    // Current active profile entity
    const activeProfile = useMemo(() => {
        return (
            profiles.find(
                (p) =>
                    String(p.id) === String(activeProfileId) ||
                    p.username === activeProfileId
            ) ||
            profiles[0] ||
            null
        );
    }, [profiles, activeProfileId]);

    // Profile actions
    const handleSelectProfile = useCallback((profileId) => {
        setActiveProfileId(profileId);
        if (typeof window.onProfileChanged === "function") {
            window.onProfileChanged(profileId);
        }
    }, []);

    const handleDeleteProfile = useCallback(
        (profileId, e) => {
            if (e && typeof e.stopPropagation === "function") {
                e.stopPropagation();
            }
            setProfiles((prev) => {
                const target = prev.find(
                    (p) => String(p.id) === String(profileId)
                );
                if (!target) return prev;
                // Do not allow deleting current active profile
                if (
                    String(target.id) === String(activeProfileId) ||
                    target.username === activeProfileId
                ) {
                    return prev;
                }
                const updated = prev.filter(
                    (p) => String(p.id) !== String(profileId)
                );
                return updated;
            });
        },
        [activeProfileId]
    );

    // Open / Close Create User Modal
    const handleOpenCreateModal = useCallback(() => {
        setNewUsername("");
        setCreateError("");
        setIsCreateUserOpen(true);
    }, []);

    const handleCloseCreateModal = useCallback(() => {
        setIsCreateUserOpen(false);
        setNewUsername("");
        setCreateError("");
    }, []);

    // Validate and submit new profile
    const handleCreateSubmit = useCallback(
        (e) => {
            if (e && typeof e.preventDefault === "function") {
                e.preventDefault();
            }
            const trimmed = newUsername.trim();

            // Validation rule: 3-20 characters, only letters, numbers, underscores
            if (!/^[a-zA-Z0-9_]{3,20}$/.test(trimmed)) {
                setCreateError("Use 3–20 letters, numbers or underscores.");
                return;
            }

            // Uniqueness validation (case-insensitive)
            const exists = profiles.some(
                (p) => p.username.toLowerCase() === trimmed.toLowerCase()
            );
            if (exists) {
                setCreateError(
                    "Username already exists. Please choose another."
                );
                return;
            }

            const newProfile = {
                id: Date.now().toString(),
                username: trimmed,
                progress: "0/12",
                createdAt: new Date().toISOString()
            };

            setProfiles((prev) => [...prev, newProfile]);
            setIsCreateUserOpen(false);
            setNewUsername("");
            setCreateError("");
        },
        [newUsername, profiles]
    );

    // Global methods for automated testing / external bridges
    const renderProfiles = useCallback((dataset, activeId) => {
        if (Array.isArray(dataset)) {
            setProfiles([...dataset]);
        }
        if (activeId !== undefined) {
            setActiveProfileId(activeId);
        }
    }, []);

    useEffect(() => {
        window.renderProfiles = renderProfiles;
        window.getProfiles = () => profiles;
        window.getActiveProfile = () => activeProfile;
        window.createProfile = (name) => {
            const trimmed = String(name || "").trim();
            if (!/^[a-zA-Z0-9_]{3,20}$/.test(trimmed)) {
                return { success: false, error: "Invalid username format" };
            }
            if (
                profiles.some(
                    (p) => p.username.toLowerCase() === trimmed.toLowerCase()
                )
            ) {
                return { success: false, error: "Username already exists" };
            }
            const newProf = {
                id: Date.now().toString(),
                username: trimmed,
                progress: "0/12"
            };
            setProfiles((prev) => [...prev, newProf]);
            return { success: true, profile: newProf };
        };
        window.switchProfile = (idOrName) => {
            const target = profiles.find(
                (p) =>
                    String(p.id) === String(idOrName) || p.username === idOrName
            );
            if (target) {
                handleSelectProfile(target.id);
                return true;
            }
            return false;
        };
        window.deleteProfile = (idOrName) => {
            const target = profiles.find(
                (p) =>
                    String(p.id) === String(idOrName) || p.username === idOrName
            );
            if (target) {
                handleDeleteProfile(target.id);
                return true;
            }
            return false;
        };
        window.openSelectProfileModal = () => setIsSelectProfileOpen(true);
        window.closeProfileModal = () => setIsSelectProfileOpen(false);
        window.openCreateUserModal = () => handleOpenCreateModal();
        window.closeCreateUserModal = () => handleCloseCreateModal();

        return () => {
            delete window.renderProfiles;
            delete window.getProfiles;
            delete window.getActiveProfile;
            delete window.createProfile;
            delete window.switchProfile;
            delete window.deleteProfile;
            delete window.openSelectProfileModal;
            delete window.closeProfileModal;
            delete window.openCreateUserModal;
            delete window.closeCreateUserModal;
        };
    }, [
        renderProfiles,
        profiles,
        activeProfile,
        handleSelectProfile,
        handleDeleteProfile,
        handleOpenCreateModal,
        handleCloseCreateModal
    ]);

    return (
        <main className="flex min-h-screen w-full items-center justify-center bg-surface-canvas p-md font-sans text-content-primary">
            {/* HUD Profile Card & Re-open Trigger when modal is closed */}
            {!isSelectProfileOpen && (
                <div className="flex flex-col items-center gap-md">
                    <ProfileCard
                        label="CURRENT PLAYER"
                        username={activeProfile?.username || "Guest"}
                        actionLabel="Switch Profile"
                        onSwitchProfile={() => setIsSelectProfileOpen(true)}
                        className="shadow-elevation-200"
                    />
                    <Button
                        label="Open Profile Manager"
                        variant="primary"
                        size="default"
                        onClick={() => setIsSelectProfileOpen(true)}
                        className="shadow-elevation-200"
                    />
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
                    <Button
                        label="Add"
                        variant="primary"
                        size="default"
                        leadingIcon={<AddIcon className="size-icon-sm" />}
                        showTrailingIcon={false}
                        onClick={handleOpenCreateModal}
                        className="min-w-[130px] border-2 border-stroke-default shadow-elevation-200 text-button-default font-bold font-sans"
                    />
                </footer>
            </Modal>

            {/* ========================================================================= */}
            {/* 2. Modal "CREATE NEW USER" (Figma Node 2282:2063) - Stacked Overlay        */}
            {/* ========================================================================= */}
            <Modal
                isOpen={isCreateUserOpen}
                onClose={handleCloseCreateModal}
                maxWidth="max-w-[460px]"
                className="w-[460px] min-h-[auto] border-2 border-stroke-default p-lg shadow-elevation-300 flex flex-col items-center gap-lg bg-surface-panel z-50"
                overlayClassName="bg-black/60 backdrop-blur-[4px] z-50"
                closeOnBackdropClick={true}
                closeOnEscape={true}
            >
                {/* Header */}
                <header className="flex w-full items-center justify-center">
                    <ModalTitle
                        as="h2"
                        className="text-center font-sans text-heading-md font-bold uppercase tracking-tight text-content-primary"
                    >
                        CREATE NEW USER
                    </ModalTitle>
                </header>

                {/* Form Body */}
                <form
                    onSubmit={handleCreateSubmit}
                    className="flex flex-col gap-lg w-full"
                >
                    <div className="w-full flex flex-col gap-xs">
                        <TextInput
                            id="create-username-input"
                            title="Username"
                            placeholder="Enter your username"
                            leadingIcon={AccountIcon}
                            value={newUsername}
                            onChange={(e) => {
                                setNewUsername(e.target.value);
                                if (createError) setCreateError("");
                            }}
                            state={createError ? "error" : "default"}
                            supportingText={
                                createError ||
                                "Use 3–20 letters, numbers or underscores."
                            }
                            autoFocus={true}
                            maxLength={20}
                            className="w-full shadow-elevation-200 border-2 border-stroke-default"
                        />
                    </div>

                    {/* Action Buttons: "✕ Cancel" and "+ Create" */}
                    <footer className="flex w-full items-center justify-between gap-md pt-xs">
                        <Button
                            label="Cancel"
                            variant="secondary"
                            size="default"
                            type="button"
                            leadingIcon={<CloseIcon className="size-icon-sm" />}
                            showTrailingIcon={false}
                            onClick={handleCloseCreateModal}
                            className="min-w-[120px] border-2 border-stroke-default shadow-elevation-200 font-bold font-sans"
                        />
                        <Button
                            label="Create"
                            variant="primary"
                            size="default"
                            type="submit"
                            leadingIcon={<AddIcon className="size-icon-sm" />}
                            showTrailingIcon={false}
                            className="min-w-[120px] border-2 border-stroke-default shadow-elevation-200 font-bold font-sans"
                        />
                    </footer>
                </form>
            </Modal>
        </main>
    );
}

export default App;
