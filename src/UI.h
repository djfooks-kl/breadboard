#pragma once

#include <glm/ext/vector_float2.hpp>
#include <ImGui/imgui.h>

class BaseApp;

typedef struct GLFWcursor GLFWcursor;
struct GLFWwindow;

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
        void UpdateMouse(flecs::world& world, BaseApp& app);

    private:

        void DrawUndo(flecs::world& world);
        void DrawDebugMenu(flecs::world& world);
        void DrawDebugInfo(flecs::world& world);
        void DrawComponentMenu(flecs::world& world, bool actionEaten);

        bool GameConsumeInput(flecs::world& world);

        ImVec2 m_PopupPosition;

        bool m_ShowDebugMenuBar = false;
        bool m_DebugInfoOpen = false;
    };
}