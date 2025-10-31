#include "UI.h"

#include <flecs/flecs.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "CameraComponent.h"
#include "Cogs/Cog.h"
#include "Cogs/CogMap.h"
#include "Core/GLFWLib.h"
#include "InputComponent.h"
#include "UIAddCogComponent.h"

void xg::UI::DrawDebugMenu(flecs::world& world)
{
    const auto& input = world.get<xg::InputComponent>();
    if (input.m_KeyDown.contains(GLFW_KEY_LEFT_CONTROL) &&
        input.m_KeyDown.contains(GLFW_KEY_LEFT_SHIFT) &&
        input.m_KeyPress.contains(GLFW_KEY_1))
    {
        m_ShowDebugMenuBar = !m_ShowDebugMenuBar;
    }

    if (m_ShowDebugMenuBar && ImGui::BeginMainMenuBar())
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

void xg::UI::DrawComponentMenu(flecs::world& world)
{
    const auto& input = world.get<xg::InputComponent>();
    const auto& cogMap = world.get<xg::CogMap>();

    world.defer_begin();
    world.each([](flecs::entity entity, xg::UIAddCogComponent)
        {
            entity.remove<xg::UIAddCogComponent>();
        });
    world.defer_end();

    xg::CogResourceId addCogId;

    ImGuiIO& io = ImGui::GetIO();

    bool openning = false;
    // Detect left mouse click anywhere in the window background
    if (!ImGui::IsPopupOpen("LeftClickPopup") &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !ImGui::IsAnyItemHovered())
    {
        // Open the popup
        ImGui::OpenPopup("LeftClickPopup");
        openning = true;
        m_PopupPosition = io.MousePos;
    }

    // Center the popup at the mouse position when it opens
    if (ImGui::BeginPopup("LeftClickPopup"))
    {
        if (!openning && !ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            ImGui::CloseCurrentPopup();

        // Optionally, move the popup to mouse position on open
        ImGui::SetWindowPos(m_PopupPosition, ImGuiCond_Always);

        if (!input.m_KeyDown.empty())
        {
            ImGui::CloseCurrentPopup();
        }

        for (const auto& itr : cogMap.GetMap())
        {
            if (ImGui::Button(itr.first.GetName()))
            {
                ImGui::CloseCurrentPopup();
                addCogId = itr.first;
            }
        }

        ImGui::EndPopup();
    }

    if (addCogId)
    {
        world.entity().ensure<xg::UIAddCogComponent>().m_CogId = addCogId;
    }
}

void xg::UI::Draw(flecs::world& world)
{
    DrawDebugMenu(world);
    DrawDebugInfo(world);
    DrawComponentMenu(world);
}
