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

    bool anyKeyDown = !input.m_KeyDown.empty();

    xg::CogResourceId addCogId;
    bool clickWindow =
        ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
        !ImGui::GetIO().WantCaptureMouse;

    if (clickWindow && m_ComponentMenuMouseClose)
    {
        m_ComponentMenuMouseClose = false;
        return;
    }

    if (!anyKeyDown && (clickWindow || m_ComponentMenuOpen))
    {
        m_ComponentMenuOpen = ImGui::BeginPopupContextVoid(nullptr, ImGuiPopupFlags_MouseButtonLeft);
        if (m_ComponentMenuOpen)
        {
            for (const auto& itr : cogMap.GetMap())
            {
                if (ImGui::Button(itr.first.GetName()))
                {
                    addCogId = itr.first;
                }
            }

            ImGui::EndPopup();
        }

        if (!m_ComponentMenuOpen)
        {
            m_ComponentMenuMouseClose = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        }
    }

    world.defer_begin();
    world.each([](flecs::entity entity, xg::UIAddCogComponent)
        {
            entity.remove<xg::UIAddCogComponent>();
        });
    world.defer_end();

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
