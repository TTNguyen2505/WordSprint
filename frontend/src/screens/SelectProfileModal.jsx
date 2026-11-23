import { useId, useState } from "react";
import AddIcon from "../assets/icons/add.svg?react";
import CloseIcon from "../assets/icons/close.svg?react";
import Button from "../components/Button";
import Modal from "../components/Modal";
import ModalTitle from "../components/ModalTitle";
import ProfileRecord from "../components/ProfileRecord";

const sampleProfiles = [
    { id: 1, name: "nguyen_02", progress: "7/12" },
    ...Array.from({ length: 6 }, (_, index) => ({
        id: index + 2,
        name: "nguyen_03",
        progress: "6/12"
    }))
];

function SelectProfileModal({
    isOpen = true,
    onClose,
    onAdd,
    onSelectProfile,
    onDeleteProfile,
    profiles: suppliedProfiles,
    activeId: suppliedActiveId
}) {
    const titleId = useId();
    const [localProfiles, setLocalProfiles] = useState(sampleProfiles);
    const [localActiveId, setLocalActiveId] = useState(sampleProfiles[0].id);
    const profiles = suppliedProfiles ?? localProfiles;
    const activeId = suppliedActiveId ?? localActiveId;

    const selectProfile = (profile) => {
        if (suppliedActiveId === undefined) setLocalActiveId(profile.id);
        onSelectProfile?.(profile);
    };

    const deleteProfile = (profile) => {
        if (suppliedProfiles === undefined) {
            setLocalProfiles((current) =>
                current.filter(({ id }) => id !== profile.id)
            );
        }
        onDeleteProfile?.(profile);
    };

    return (
        <Modal
            isOpen={isOpen}
            onClose={onClose}
            aria-labelledby={titleId}
            maxWidth="max-w-[620px]"
            header={
                <ModalTitle
                    id={titleId}
                    className="w-full text-center text-[length:var(--primitive-font-size-36)] leading-[var(--primitive-line-height-40)]"
                >
                    SELECT A PROFILE
                </ModalTitle>
            }
            body={
                <div
                    role="table"
                    aria-label="Profiles"
                    className="max-h-[400px] w-max overflow-y-auto"
                >
                    <ProfileRecord
                        type="Header"
                        className="sticky top-0 z-10"
                    />
                    {profiles.map((profile) => (
                        <ProfileRecord
                            key={profile.id}
                            type={
                                profile.id === activeId ? "Active" : "Default"
                            }
                            playerName={profile.name}
                            progress={profile.progress}
                            onSelect={
                                profile.id === activeId
                                    ? undefined
                                    : () => selectProfile(profile)
                            }
                            onDelete={() => deleteProfile(profile)}
                            deleteAriaLabel={`Delete ${profile.name} profile`}
                        />
                    ))}
                </div>
            }
            footer={
                <div className="flex gap-2xl">
                    <Button
                        label="Close"
                        size="default"
                        variant="secondary"
                        leadingIcon={<CloseIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={onClose}
                    />
                    <Button
                        label="Add"
                        size="default"
                        leadingIcon={<AddIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={onAdd}
                    />
                </div>
            }
        />
    );
}

export default SelectProfileModal;
