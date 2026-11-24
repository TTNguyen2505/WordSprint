import { useAppSessionStore } from "./appSessionStore";

export function useProfileViewModel() {
    const profiles = useAppSessionStore((state) => state.profiles);
    const activeId = useAppSessionStore((state) => state.activeId);
    const switchProfile = useAppSessionStore((state) => state.switchProfile);
    return { profiles, activeId, switchProfile };
}
