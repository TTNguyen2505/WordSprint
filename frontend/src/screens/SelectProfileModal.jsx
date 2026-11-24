import { useId } from "react";
import AddIcon from "../assets/icons/add.svg?react";
import CloseIcon from "../assets/icons/close.svg?react";
import Button from "../components/Button";
import Modal from "../components/Modal";
import ModalTitle from "../components/ModalTitle";
import ProfileRecord from "../components/ProfileRecord";
import { useProfileViewModel } from "../viewModels/profileViewModel";

function SelectProfileModal({ isOpen = true, onClose, onAdd }) {
    const titleId = useId();
    const { profiles, activeId, switchProfile } = useProfileViewModel();

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
                                    : () => switchProfile(profile.id)
                            }
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
