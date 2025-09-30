#include "UI.h"

#include <flecs/flecs.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "CameraComponent.h"
#include "Core/GLFWLib.h"
#include "InputComponent.h"

void xg::UI::DrawMenu(flecs::world& world)
{
    world.query<const xg::InputComponent>().each([&](
        const xg::InputComponent& input)
        {
            if (input.m_KeyDown.contains(GLFW_KEY_LEFT_CONTROL) &&
                input.m_KeyDown.contains(GLFW_KEY_LEFT_SHIFT) &&
                input.m_KeyPress.contains(GLFW_KEY_1))
            {
                m_ShowMenuBar = !m_ShowMenuBar;
            }
        });

    if (m_ShowMenuBar && ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Info"))
            {
                m_DebugInfoOpen = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void xg::UI::DrawDebugInfo(flecs::world& world)
{
    // scale window to fit contents
    ImGui::SetNextWindowSize(ImVec2{ 0.f, 0.f });
    if (m_DebugInfoOpen && ImGui::Begin("Debug Info", &m_DebugInfoOpen))
    {
        world.query<const xg::CameraComponent>()
            .each([&](
                const xg::CameraComponent& camera)
                {
                    ImGui::Text("Zoom %.3f", camera.m_Zoom);
                    ImGui::Text("Pos %.3f %.3f", camera.m_Position.x, camera.m_Position.y);
                });
        ImGui::End();
    }
}

void xg::UI::Draw(flecs::world& world)
{
    DrawMenu(world);
    DrawDebugInfo(world);
}
