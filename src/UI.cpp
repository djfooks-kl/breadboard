#include "UI.h"

#include <flecs/flecs.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "CameraComponent.h"
#include "CameraHelpers.h"
#include "Cogs/Cog.h"
#include "Cogs/CogMap.h"
#include "Core/GLFWLib.h"
#include "InputComponent.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIRedoComponent.h"
#include "UIUndoComponent.h"
#include "WorldMouseComponent.h"

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
        world.each([&](const xg::CameraComponent& camera)
                {
                    ImGui::Text("Zoom %.3f", camera.m_Zoom);
                    ImGui::Text("Pos %.3f %.3f", camera.m_Position.x, camera.m_Position.y);
                });
        ImGui::End();
    }
}

void xg::UI::DrawComponentMenu(flecs::world& world)
{
    const auto& camera = world.get<xg::CameraComponent>();
    const auto& cogMap = world.get<xg::CogMap>();
    const auto& input = world.get<xg::InputComponent>();
    const auto& worldMouse = world.get<xg::WorldMouseComponent>();

    xg::CogResourceId addCogId;
    xg::CogResourceId hoverCogId;

    ImGuiIO& io = ImGui::GetIO();

    bool openning = false;
    if (!ImGui::IsPopupOpen("LeftClickPopup") &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !ImGui::IsAnyItemHovered())
    {
        ImGui::OpenPopup("LeftClickPopup");
        openning = true;
        m_PopupPosition = io.MousePos;

        constexpr glm::vec2 popupPreviewOffset(-0.5f, 0.2f);
        m_PopupPreviewWorldPosition = worldMouse.m_Position + popupPreviewOffset;
    }

    if (ImGui::BeginPopup("LeftClickPopup"))
    {
        if (!openning && !ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            ImGui::CloseCurrentPopup();

        ImGui::SetWindowPos(m_PopupPosition, ImGuiCond_Always);

        if (!input.m_KeyDown.empty())
        {
            ImGui::CloseCurrentPopup();
        }

        for (const auto& itr : cogMap.GetMap())
        {
            if (ImGui::Button(itr.first.GetName(), ImVec2(100.f, 0.f)))
            {
                ImGui::CloseCurrentPopup();
                addCogId = itr.first;
            }
            if (ImGui::IsItemHovered())
            {
                hoverCogId = itr.first;
            }
        }

        ImGui::EndPopup();
    }

    world.defer_begin();
    world.each([](flecs::entity entity, xg::UIPreviewAddingCogComponent& addingCog)
        {
            if (!addingCog.m_Hovering)
            {
                entity.remove<xg::UIPreviewAddingCogComponent>();
            }
        });
    world.defer_end();

    if (addCogId)
    {
        auto& addingCog = world.entity().ensure<xg::UIPreviewAddingCogComponent>();
        addingCog.m_CogId = addCogId;
        addingCog.m_Hovering = false;
    }
    else if (hoverCogId)
    {
        auto& addingCog = world.entity().ensure<xg::UIPreviewAddingCogComponent>();
        addingCog.m_CogId = hoverCogId;
        addingCog.m_PreviewPosition = glm::vec2(m_PopupPreviewWorldPosition.x, m_PopupPreviewWorldPosition.y);
        addingCog.m_Hovering = true;
    }
    else
    {
        world.defer_begin();
        world.each([](flecs::entity entity, xg::UIPreviewAddingCogComponent&)
            {
                entity.remove<xg::UIPreviewAddingCogComponent>();
            });
        world.defer_end();
    }
}

void xg::UI::DrawUndo(flecs::world& world)
{
    world.defer_begin();
    world.each([](flecs::entity entity, xg::UIUndoComponent)
        {
            entity.destruct();
        });

    world.each([](flecs::entity entity, xg::UIRedoComponent)
        {
            entity.destruct();
        });
    world.defer_end();

    const auto& input = world.get<xg::InputComponent>();
    if (input.m_KeyDown.contains(GLFW_KEY_LEFT_CONTROL) &&
        input.m_KeyPress.contains(GLFW_KEY_Z))
    {
        world.entity().add<xg::UIUndoComponent>();
    }

    if (input.m_KeyDown.contains(GLFW_KEY_LEFT_CONTROL) &&
        input.m_KeyPress.contains(GLFW_KEY_Y))
    {
        world.entity().add<xg::UIRedoComponent>();
    }
}

void xg::UI::Draw(flecs::world& world)
{
    DrawUndo(world);
    DrawDebugMenu(world);
    DrawDebugInfo(world);
    DrawComponentMenu(world);
}
