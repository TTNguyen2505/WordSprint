import Slider from "@/components/primitives/Slider";

function AudioSettings({
    musicEnabled,
    musicVolume,
    soundEnabled,
    soundVolume,
    onMusicToggle,
    onMusicVolumeChange,
    onSoundToggle,
    onSoundVolumeChange
}) {
    return (
        <div className="flex w-[640px] flex-col gap-xl">
            <Slider
                title={`Background Music (${musicVolume}%)`}
                enabled={musicEnabled}
                value={musicVolume}
                className="max-w-none gap-xl"
                onToggle={onMusicToggle}
                onChange={onMusicVolumeChange}
            />
            <Slider
                title={`Sound Effect (${soundVolume}%)`}
                enabled={soundEnabled}
                value={soundVolume}
                className="max-w-none gap-xl"
                onToggle={onSoundToggle}
                onChange={onSoundVolumeChange}
            />
        </div>
    );
}

export default AudioSettings;
