#include "DebugUI.h"

#include <flecs/flecs.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <format>
#include <sstream>
#include <set>

#include "CameraComponent.h"
#include "CameraHelpers.h"
#include "CogComponent.h"
#include "Command/CommandAddCogComponent.h"
#include "Command/CommandAddWireComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandDeleteWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandListComponent.h"
#include "Command/CommandUndoComponent.h"
#include "Core/GLFWLib.h"
#include "EntityHash.h"
#include "GameComponent.h"
#include "GridAttachmentsComponent.h"
#include "InputComponent.h"
#include "UIHoverComponent.h"
#include "WireComponent.h"
#include "WorldMouseComponent.h"

namespace
{
    std::string GetEntityDetails(const flecs::entity& entity);

    std::string GetEntityHandleDetails(const flecs::entity& entity)
    {
        return std::format("{} {}", entity.id(), GetEntityDetails(entity));
    }

    std::string GetEntityDetails(const flecs::entity& entity)
    {
        if (!entity.is_valid())
        {
            return "invalid";
        }
        if (!entity.is_alive())
        {
            return "destroyed";
        }
        if (const auto* wireComponent = entity.try_get<xg::WireComponent>())
        {
            return std::format("Wire [{},{}] ... [{},{}]",
                wireComponent->m_Checkpoints.front().x,
                wireComponent->m_Checkpoints.front().y,
                wireComponent->m_Checkpoints.back().x,
                wireComponent->m_Checkpoints.back().y);
        }
        if (const auto* cogComponent = entity.try_get<xg::CogComponent>())
        {
            return std::format("{} [{}, {}]",
                cogComponent->m_CogId.GetName(),
                cogComponent->m_Transform.m_Translation.x,
                cogComponent->m_Transform.m_Translation.y);
        }
        if (const auto* command = entity.try_get<xg::command::AddCogComponent>())
        {
            if (const auto* commandEntity = entity.try_get<xg::command::EntityComponent>())
            {
                return std::format("Add {}", GetEntityHandleDetails(commandEntity->m_Entity));
            }
            else
            {
                return std::format("Add {}", command->m_CogId.GetName());
            }
        }
        if (const auto* command = entity.try_get<xg::command::DeleteCogComponent>())
        {
            return std::format("Delete {}", GetEntityHandleDetails(command->m_Cog));
        }
        if (const auto* command = entity.try_get<xg::command::AddWireComponent>())
        {
            if (const auto* commandEntity = entity.try_get<xg::command::EntityComponent>())
            {
                return std::format("Add {}", GetEntityHandleDetails(commandEntity->m_Entity));
            }
            else
            {
                return "Add wire";
            }
        }
        if (const auto* command = entity.try_get<xg::command::DeleteWireComponent>())
        {
            return std::format("Delete {}", GetEntityHandleDetails(command->m_Wire));
        }

        std::stringstream ss;
        entity.each([&](flecs::id id)
        {
            flecs::entity component = id.entity();
            ss << component.name() ? component.name() : "<unnamed>";
            ss << ",";
        });
        return ss.str();
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
                m_CameraOpen = true;
            }
            if (ImGui::MenuItem("Command History"))
            {
                m_CommandHistoryOpen = true;
            }
            if (ImGui::MenuItem("Grid"))
            {
                m_GridOpen = true;
            }
            if (ImGui::MenuItem("Sanity Check"))
            {
                m_SanityCheckOpen = true;
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
    if (m_CameraOpen && ImGui::Begin("Debug Camera", &m_CameraOpen))
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
    if (m_GridOpen && ImGui::Begin("Debug Grid", &m_GridOpen))
    {
        const auto& hover = world.get<const xg::UIHoverComponent>();
        ImGui::Text("Hovered cog: %s", GetEntityHandleDetails(hover.m_Cog).c_str());

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

        ImGui::Text("Has node:       %s", attachments.m_HasNode ? "True" : "False");
        ImGui::Text("Has checkpoint: %s", attachments.m_HasWireCheckpoint ? "True" : "False");
        if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent(10.f);
            for (const flecs::entity& entity : attachments.m_Entities)
            {
                ImGui::Text("%-5d %s", entity.id(), GetEntityHandleDetails(entity).c_str());
            }
            ImGui::Unindent(10.f);
        }
    }
    ImGui::Unindent(10.f);
}

void xg::DebugUI::DrawCommandHistoryWindow(flecs::world& world)
{
    ImGui::SetNextWindowSize(ImVec2{ 300.f, 600.f }, ImGuiCond_FirstUseEver);
    if (m_CommandHistoryOpen && ImGui::Begin("Debug Command History", &m_CommandHistoryOpen))
    {
        const auto& listComponent = world.get<const xg::command::ListComponent>();
        ImGui::Text("Count: %d", listComponent.m_Count);
        ImGui::Text("Undo Count: %d", listComponent.m_UndoCount);
        for (int i = 0; i < listComponent.m_Commands.size(); ++i)
        {
            const bool isHead = i == listComponent.m_HeadIndex;
            const bool isUndoHead = i == listComponent.m_UndoHeadIndex;
            ImGui::Text(std::format("{:5} {} {}",
                i,
                isHead ? "[H]" : isUndoHead ? "[U]" : "   ",
                GetEntityDetails(listComponent.m_Commands[i])).c_str());
        }
        ImGui::End();
    }
}

void xg::DebugUI::DrawSanityCheckWindow(flecs::world& world)
{
    ImGui::SetNextWindowSize(ImVec2{ 300.f, 600.f }, ImGuiCond_FirstUseEver);
    if (m_SanityCheckOpen && ImGui::Begin("Sanity Check", &m_SanityCheckOpen))
    {
        std::set<flecs::entity> set;
        world.each([&](flecs::entity entity, const xg::GameComponent&)
        {
            set.insert(entity);
        });

        const auto& listComponent = world.get<const xg::command::ListComponent>();
        for (const flecs::entity& commandEntity : listComponent.m_Commands)
        {
            if (!commandEntity.is_valid())
                continue;

            set.erase(commandEntity);
            if (const auto* entityComponent = commandEntity.try_get<const xg::command::EntityComponent>())
            {
                set.erase(entityComponent->m_Entity);
            }
            if (const auto* undoComponent = commandEntity.try_get<const xg::command::UndoComponent>())
            {
                set.erase(undoComponent->m_Undo);
            }
        }
        const xg::GridAttachmentsMap& gridMap = world.get<const xg::GridAttachmentsComponent>().m_Map;
        for (const auto& pair : gridMap)
        {
            for (const flecs::entity& entity : pair.second.m_Entities)
            {
                set.erase(entity);
            }
        }

        if (set.empty())
        {
            ImGui::Text("No leaked entities");
        }
        else
        {
            ImGui::Text("Possible leaked entity:");
            int i = 0;
            for (const flecs::entity& entity : set)
            {
                ImGui::PushID(i);
                if (ImGui::CollapsingHeader(GetEntityHandleDetails(entity).c_str()))
                {
                    const std::source_location& source = entity.get<const xg::GameComponent>().m_CreatedAt;
                    ImGui::Text("%s:%d", source.file_name(), source.line());
                }
                ImGui::PopID();
                ++i;
            }
        }

        ImGui::End();
    }
}

void xg::DebugUI::Draw(flecs::world& world)
{
    DrawMenu(world);

    DrawCameraWindow(world);
    DrawCommandHistoryWindow(world);
    DrawGridWindow(world);
    DrawSanityCheckWindow(world);
}
