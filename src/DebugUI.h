#pragma once

#include <optional>
#include <glm/ext/vector_int2.hpp>

namespace flecs
{
    struct world;
}

namespace xg
{
    class DebugUI
    {
    public:
        void Draw(flecs::world& world);
        bool GameConsumeInput(flecs::world& world);

    private:

        void DrawMenu(flecs::world& world);
        void DrawCameraWindow(flecs::world& world);
        void DrawGridWindow(flecs::world& world);
        void DrawCommandHistoryWindow(flecs::world& world);
        void DrawSanityCheckWindow(flecs::world& world);

        void DrawGridCell(flecs::world& world, const glm::ivec2& cell);

        bool m_ShowDebugMenuBar = false;
        bool m_CameraOpen = false;
        bool m_GridOpen = false;
        bool m_CommandHistoryOpen = false;
        bool m_SanityCheckOpen = false;

        bool m_CaptureSelectedCell = false;
        std::optional<glm::ivec2> m_SelectedCell;
    };
}