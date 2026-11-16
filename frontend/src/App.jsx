import TextInput from "@/components/TextInput.jsx";

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

function App() {
    return (
        <main className="grid min-h-screen place-items-center bg-surface-canvas px-md py-lg font-sans text-content-primary sm:p-lg">
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
        </main>
    );
}

export default App;
