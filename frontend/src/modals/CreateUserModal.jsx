import { useId, useState } from "react";
import AddIcon from "../assets/icons/add.svg?react";
import CloseIcon from "../assets/icons/close.svg?react";
import Button from "../components/primitives/Button";
import Modal from "../components/primitives/Modal";
import ModalTitle from "../components/primitives/ModalTitle";
import TextInput from "../components/primitives/TextInput";

function CreateUserModal({ isOpen = true, onClose, onCreate }) {
    const titleId = useId();
    const formId = useId();
    const [username, setUsername] = useState("");
    const [hasError, setHasError] = useState(false);
    const [serverError, setServerError] = useState("");
    const [isSaving, setIsSaving] = useState(false);

    const handleSubmit = async (event) => {
        event.preventDefault();
        if (!/^[a-zA-Z0-9_]{3,20}$/.test(username)) {
            setHasError(true);
            return;
        }
        setHasError(false);
        setServerError("");
        setIsSaving(true);
        try {
            await onCreate?.(username);
        } catch (error) {
            setServerError(error.message);
        } finally {
            setIsSaving(false);
        }
    };

    return (
        <Modal
            isOpen={isOpen}
            onClose={onClose}
            aria-labelledby={titleId}
            maxWidth="max-w-[580px]"
            header={<ModalTitle id={titleId}>CREATE NEW USER</ModalTitle>}
            body={
                <form id={formId} onSubmit={handleSubmit} noValidate>
                    <TextInput
                        className="w-[480px]"
                        title="Username"
                        placeholder="Enter your username"
                        value={username}
                        onChange={(event) => {
                            setUsername(event.target.value);
                            setHasError(false);
                            setServerError("");
                        }}
                        state={hasError ? "error" : "default"}
                        maxLength={20}
                        autoComplete="off"
                    />
                    {serverError && <p role="alert">{serverError}</p>}
                </form>
            }
            footer={
                <div className="flex gap-2xl">
                    <Button
                        label="Cancel"
                        size="default"
                        variant="secondary"
                        leadingIcon={<CloseIcon className="size-full" />}
                        showTrailingIcon={false}
                        onClick={onClose}
                    />
                    <Button
                        label="Create"
                        size="default"
                        type="submit"
                        form={formId}
                        disabled={isSaving}
                        leadingIcon={<AddIcon className="size-full" />}
                        showTrailingIcon={false}
                    />
                </div>
            }
        />
    );
}

export default CreateUserModal;
