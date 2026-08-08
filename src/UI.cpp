#include "UI.h"

#include <flecs/flecs.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <iostream>

#include "BaseApp.h"
#include "CameraComponent.h"
#include "CameraHelpers.h"
#include "Cogs/CogMap.h"
#include "Cogs/CogPrototype.h"
#include "Core/GLFWLib.h"
#include "DebugUI.h"
#include "FlecsGame.h"
#include "InputComponent.h"
#include "MouseCursorEnum.h"
#include "UIDeleteCogComponent.h"
#include "UIDeleteWireComponent.h"
#include "UIDraggingDropComponent.h"
#include "UIHoverComponent.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIPreviewAddingWireComponent.h"
#include "UIPreviewCreateWireComponent.h"
#include "UIRedoComponent.h"
#include "UIRotateComponent.h"
#include "UIUndoComponent.h"
#include "WorldMouseComponent.h"

namespace
{
    void UpdateRotate(flecs::world& world)
    {
        const auto& input = world.get<xg::InputComponent>();
        auto& rotate = world.get_mut<xg::UIRotateComponent>();
        rotate.m_RotationDirection = input.m_KeyPress.contains(GLFW_KEY_R) ? 1 : 0;
    }

    void UpdateDelete(flecs::world& world)
    {
        world.defer_begin();
        world.each([](flecs::entity entity, xg::UIDeleteCogComponent)
            {
                entity.destruct();
            });
        world.defer_end();

        world.defer_begin();
        world.each([](flecs::entity entity, xg::UIDeleteWireComponent)
            {
                entity.destruct();
            });
        world.defer_end();

        const auto& input = world.get<xg::InputComponent>();
        if (input.m_KeyPress.contains(GLFW_KEY_DELETE) ||
            input.m_KeyPress.contains(GLFW_KEY_BACKSPACE))
        {
            auto& hover = world.get<xg::UIHoverComponent>();
            if (hover.m_Cog)
            {
                xg::CreateEntity(world).ensure<xg::UIDeleteCogComponent>().m_Cog = hover.m_Cog;
            }
            if (hover.m_Wire)
            {
                xg::CreateEntity(world).ensure<xg::UIDeleteWireComponent>().m_Wire = hover.m_Wire;
            }
        }
    }
}

xg::UI::UI()
    : m_DebugUI(std::make_unique<xg::DebugUI>())
{
}

xg::UI::~UI() = default;

void xg::UI::UpdateMouse(flecs::world& world, BaseApp& app)
{
    auto& previewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
    const auto& hover = world.get<xg::UIHoverComponent>();
    bool hoverWire = previewAddingCog.m_AddCogId.IsEmpty() && (hover.m_Node || hover.m_Wire);
    app.SetCursor(hoverWire ? EMouseCursor::Cross : EMouseCursor::Arrow);
}

void xg::UI::DrawCogMenu(flecs::world& world, const bool actionEaten)
{
    const auto& cogMap = world.get<xg::CogMap>();
    const auto& input = world.get<xg::InputComponent>();
    const auto& worldMouse = world.get<xg::WorldMouseComponent>();
    auto& previewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
    auto& dragDrop = world.get_mut<xg::UIDraggingDropComponent>();

    if (previewAddingCog.m_AddCogId)
    {
        dragDrop.m_Drop = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        if (dragDrop.m_Drop || input.m_KeyDown.contains(GLFW_KEY_DELETE))
        {
            previewAddingCog.m_AddCogId = xg::CogResourceId();
        }
        return;
    }
    dragDrop.m_Drop = false;

    xg::CogResourceId addCogId;
    xg::CogResourceId hoverCogId;

    ImGuiIO& io = ImGui::GetIO();

    bool openning = false;
    if (!ImGui::IsPopupOpen("LeftClickPopup") &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !actionEaten &&
        !ImGui::IsAnyItemHovered() &&
        !ImGui::IsWindowHovered() &&
        !io.WantCaptureMouse)
    {
        ImGui::OpenPopup("LeftClickPopup");
        openning = true;
        m_PopupPosition = io.MousePos;

        constexpr glm::vec2 popupPreviewOffset(-0.5f, 0.2f);
        previewAddingCog.m_PreviewPosition = worldMouse.m_Position + popupPreviewOffset;
    }

    m_CogPopupOpen = ImGui::BeginPopup("LeftClickPopup");
    if (m_CogPopupOpen)
    {
        if (!openning && !ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            ImGui::CloseCurrentPopup();
            m_CogPopupOpen = false;
        }

        ImGui::SetWindowPos(m_PopupPosition, ImGuiCond_Always);

        if (!input.m_KeyDown.empty())
        {
            ImGui::CloseCurrentPopup();
            m_CogPopupOpen = false;
        }

        for (const auto& itr : cogMap.GetMap())
        {
            if (ImGui::Button(itr.first.GetName(), ImVec2(100.f, 0.f)))
            {
                ImGui::CloseCurrentPopup();
                m_CogPopupOpen = false;
                addCogId = itr.first;
            }
            else if (ImGui::IsItemHovered())
            {
                hoverCogId = itr.first;
            }
        }

        ImGui::EndPopup();
    }

    previewAddingCog.m_AddCogId = addCogId;
    previewAddingCog.m_HoverCogId = hoverCogId;
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
        xg::CreateEntity(world).add<xg::UIUndoComponent>();
    }

    if (input.m_KeyDown.contains(GLFW_KEY_LEFT_CONTROL) &&
        input.m_KeyPress.contains(GLFW_KEY_Y))
    {
        xg::CreateEntity(world).add<xg::UIRedoComponent>();
    }
}

bool xg::UI::GameConsumeInput(flecs::world& world)
{
    auto& previewAddingWire = world.get_mut<xg::UIPreviewAddingWireComponent>();
    auto& createWire = world.get_mut<xg::UIPreviewCreateWireComponent>();
    if (createWire.m_Create)
    {
        previewAddingWire.m_Active = false;
        createWire.m_Create = false;
    }

    const bool doAction = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    if (!doAction)
    {
        return false;
    }

    if (m_DebugUI->GameConsumeInput(world))
    {
        return true;
    }

    if (m_CogPopupOpen &&
        ImGui::IsAnyItemHovered())
    {
        return false;
    }

    if (previewAddingWire.m_Active)
    {
        createWire.m_Create = true;
        return true;
    }

    auto& hover = world.get<xg::UIHoverComponent>();
    if (hover.m_Node || hover.m_Wire)
    {
        previewAddingWire.m_Active = true;
        return true;
    }

    return false;
}

void xg::UI::Draw(flecs::world& world)
{
    DrawUndo(world);

    m_DebugUI->Draw(world);

    UpdateRotate(world);
    UpdateDelete(world);

    const bool actionEaten = GameConsumeInput(world);
    DrawCogMenu(world, actionEaten);
}
