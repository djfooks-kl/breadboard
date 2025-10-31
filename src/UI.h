#pragma once

#include <ImGui/imgui.h>

namespace flecs
{
    struct world;
}

namespace xg
{
    class UI
    {
    public:
        void Draw(flecs::world& world);

    private:
        void DrawDebugMenu(flecs::world& world);
        void DrawDebugInfo(flecs::world& world);

        void DrawComponentMenu(flecs::world& world);

        ImVec2 m_PopupPosition;

        bool m_ShowDebugMenuBar = false;
        bool m_DebugInfoOpen = false;
    };
}