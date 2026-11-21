import TextInput from "@/components/TextInput.jsx";
import Button from "@/components/Button.jsx";
import { useQtBridge } from "@/hooks/useQtBridge.js";

const states = [
    {
        id: "default",
        placeholder: "Placeholder",
        supportingText: "Use 3–20 letters, numbers or underscores."
    },
    {
        id: "focus",
        inputText: "Input Text",
        supportingText: "Use 3–20 letters, numbers or underscores."
    },
    {
        id: "error",
        inputText: "Input Text",
        supportingText: "Something was wrong"
    },
    {
        id: "success",
        inputText: "Input Text",
        supportingText: "Acceptable"
    }
];

const buttonSizes = ["small", "default", "large"];
const buttonVariants = [
    "primary",
    "secondary",
    "destructive",
    "inverse",
    "disabled"
];
const buttonStates = ["default", "hover", "pressed"];

function App() {
    useQtBridge();

    return (
        <main className="flex min-h-screen flex-col items-center gap-2xl bg-surface-canvas px-md py-lg font-sans text-content-primary sm:p-lg">
            <section
                className="flex min-w-0 w-[min(480px,100%)] flex-col gap-lg"
                aria-label="Text input component states"
            >
                {states.map(({ id, ...props }) => (
                    <TextInput
                        key={id}
                        id={`input-${id}`}
                        state={id}
                        {...props}
                    />
                ))}
                <TextInput
                    id="input-compact-error"
                    state="error"
                    inputText="Input Text"
                    showTitle={false}
                    showSupportingText={false}
                    aria-label="Text input without title or supporting text"
                />
            </section>
            <section
                className="flex w-[min(760px,100%)] flex-col gap-lg"
                aria-label="Button component variants"
            >
                {buttonSizes.map((size) => (
                    <div key={size} className="flex flex-col gap-sm">
                        <h2 className="text-heading-sm m-0 capitalize">
                            {size}
                        </h2>
                        <div className="grid grid-cols-3 gap-sm">
                            {buttonVariants.map((variant) => {
                                const statesToShow =
                                    variant === "disabled"
                                        ? ["default"]
                                        : buttonStates;

                                return (
                                    <div
                                        key={variant}
                                        className="col-span-3 grid grid-cols-subgrid items-center justify-items-start gap-sm"
                                    >
                                        {statesToShow.map((state) => (
                                            <Button
                                                key={`${variant}-${state}`}
                                                label={variant}
                                                size={size}
                                                state={state}
                                                variant={variant}
                                            />
                                        ))}
                                    </div>
                                );
                            })}
                        </div>
                    </div>
                ))}
            </section>
        </main>
    );
}

export default App;
