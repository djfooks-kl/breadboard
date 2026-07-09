#include "DebugUI.h"

#include <flecs/flecs.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include "Core/GLFWLib.h"
#include "CameraHelpers.h"
#include "CogComponent.h"
#include "InputComponent.h"
#include "WireComponent.h"
#include "WorldMouseComponent.h"
#include "CameraComponent.h"
#include "GridAttachmentsComponent.h"

namespace
{
    const char* GetCellEntityString(const flecs::entity& entity)
    {
        if (entity.has<xg::WireComponent>())
        {
            return "Wire";
        }
        if (const auto* cogComponent = entity.try_get<xg::CogComponent>())
        {
            return cogComponent->m_CogId.GetName();
        }
        return "Unknown";
    }
}

bool xg::DebugUI::GameConsumeInput(flecs::world& world)
{
    if (m_CaptureSelectedCell)
    {
        m_SelectedCell = world.get<xg::WorldMouseComponent>().m_Cell;
        m_CaptureSelectedCell = false;
        return true;
    }

    return false;
}

void xg::DebugUI::DrawMenu(flecs::world& world)
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
            if (ImGui::MenuItem("Camera"))
            {
                m_DebugCameraOpen = true;
            }
            if (ImGui::MenuItem("Grid"))
            {
                m_DebugGridOpen = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void xg::DebugUI::DrawCameraWindow(flecs::world& world)
{
    // scale window to fit contents
    ImGui::SetNextWindowSize(ImVec2{ 0.f, 0.f });
    if (m_DebugCameraOpen && ImGui::Begin("Debug Camera", &m_DebugCameraOpen))
    {
        world.each([&](const xg::CameraComponent& camera)
                {
                    ImGui::Text("Zoom %.3f", camera.m_Zoom);
                    ImGui::Text("Pos %.3f %.3f", camera.m_Position.x, camera.m_Position.y);
                });
        ImGui::End();
    }
}

void xg::DebugUI::DrawGridWindow(flecs::world& world)
{
    ImGui::SetNextWindowSize(ImVec2{ 300.f, 600.f }, ImGuiCond_FirstUseEver);
    if (m_DebugGridOpen && ImGui::Begin("Debug Grid", &m_DebugGridOpen))
    {
        ImGui::Checkbox("Select cell", &m_CaptureSelectedCell);
        if (m_SelectedCell.has_value())
        {
            ImGui::SameLine();
            if (ImGui::Button("Debug hovered cell"))
            {
                m_SelectedCell = std::nullopt;
            }
        }

        if (m_SelectedCell.has_value())
        {
            DrawGridCell(world, m_SelectedCell.value());
        }
        else
        {
            DrawGridCell(world, world.get<xg::WorldMouseComponent>().m_Cell);
        }
        ImGui::End();
    }
}

void xg::DebugUI::DrawGridCell(flecs::world& world, const glm::ivec2& cell)
{
    ImGui::Text("x: %-4d, y: %-4d", cell.x, cell.y);
    ImGui::Indent(10.f);
    const auto& map = world.get<xg::GridAttachmentsComponent>().m_Map;
    auto itr = map.find(cell);
    if (itr == map.end())
    {
        ImGui::Text("Empty");
    }
    else
    {
        const auto& attachments = itr->second;

        ImGui::Text("Has node: %s", attachments.m_HasNode ? "True" : "False");
        ImGui::Text("Has dot:  %s", attachments.m_HasWireDot ? "True" : "False");
        if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent(10.f);
            for (const flecs::entity& entity : attachments.m_Entities)
            {
                ImGui::Text("%-5d %s", entity.id(), GetCellEntityString(entity));
            }
            ImGui::Unindent(10.f);
        }
    }
    ImGui::Unindent(10.f);
}

void xg::DebugUI::Draw(flecs::world& world)
{
    DrawMenu(world);
    DrawCameraWindow(world);
    DrawGridWindow(world);
}
