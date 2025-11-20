#pragma once

#include <glm/ext/vector_float2.hpp>
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
        void DrawUndo(flecs::world& world);
        void DrawDebugMenu(flecs::world& world);
        void DrawDebugInfo(flecs::world& world);

        void DrawComponentMenu(flecs::world& world);

        ImVec2 m_PopupPosition;
        glm::vec2 m_PopupPreviewWorldPosition;

        bool m_ShowDebugMenuBar = false;
        bool m_DebugInfoOpen = false;
    };
}