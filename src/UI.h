#pragma once

#include <glm/ext/vector_float2.hpp>
#include <ImGui/imgui.h>
#include <memory>

class BaseApp;

typedef struct GLFWcursor GLFWcursor;
struct GLFWwindow;

namespace flecs
{
    struct world;
}

namespace xg
{
    class DebugUI;

    class UI
    {
    public:
        UI();
        ~UI();

        void Draw(flecs::world& world);
        void UpdateMouse(flecs::world& world, BaseApp& app);

    private:

        void DrawUndo(flecs::world& world);

        void DrawCogMenu(flecs::world& world, bool actionEaten);

        bool GameConsumeInput(flecs::world& world);

        ImVec2 m_PopupPosition;

        std::unique_ptr<xg::DebugUI> m_DebugUI;

        bool m_ShowDebugMenuBar = false;
        bool m_DebugCameraOpen = false;
        bool m_DebugGridOpen = false;
    };
}