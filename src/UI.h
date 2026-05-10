#pragma once

#include <glm/ext/vector_float2.hpp>
#include <ImGui/imgui.h>

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
        UI();
        ~UI();

        void Draw(flecs::world& world);
        void UpdateMouse(flecs::world& world, GLFWwindow* window);

    private:

        void DrawUndo(flecs::world& world);
        void DrawDebugMenu(flecs::world& world);
        void DrawDebugInfo(flecs::world& world);
        void DrawComponentMenu(flecs::world& world);

        GLFWcursor* m_CrossCursor = nullptr;

        ImVec2 m_PopupPosition;

        bool m_ShowDebugMenuBar = false;
        bool m_DebugInfoOpen = false;
    };
}