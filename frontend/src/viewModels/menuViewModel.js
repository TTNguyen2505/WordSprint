import { useAppSessionStore } from "./appSessionStore";

export function useMenuViewModel() {
    const profiles = useAppSessionStore((state) => state.profiles);
    const activeId = useAppSessionStore((state) => state.activeId);
    return profiles.find((profile) => profile.id === activeId) ?? null;
}
