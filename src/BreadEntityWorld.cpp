#include "BreadEntityWorld.h"

#include <flecs/flecs.h>

#include "CameraComponent.h"
#include "CameraInputComponent.h"
#include "CameraInputSystem.h"
#include "CameraSystem.h"
#include "CogCreatedComponent.h"
#include "Cogs/BatterySystem.h"
#include "Cogs/CogMap.h"
#include "Cogs/Register.h"
#include "CogSystem.h"
#include "Command/CommandCreateSystem.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandListComponent.h"
#include "Command/CommandListSystem.h"
#include "Command/CommandRemovedFromHistoryComponent.h"
#include "GlobalComponent.h"
#include "GridAttachmentsComponent.h"
#include "GridAttachmentSystem.h"
#include "GridSizeComponent.h"
#include "InputComponent.h"
#include "InputSystem.h"
#include "OnStageAddedComponent.h"
#include "OnStageRemovedComponent.h"
#include "OnStageSystem.h"
#include "RenderSettings.h"
#include "UIDragDropSystem.h"
#include "UIDraggingDropComponent.h"
#include "UIDragPreviewSystem.h"
#include "UIDragValidationSystem.h"
#include "UIDragValidComponent.h"
#include "UIHoverComponent.h"
#include "UIHoverSystem.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIPreviewAddingWireComponent.h"
#include "UIPreviewCreateWireComponent.h"
#include "UIPreviewWireSystem.h"
#include "UIRedoComponent.h"
#include "UIRotateComponent.h"
#include "UISettings.h"
#include "UIUndoComponent.h"
#include "UIWirePathfindingSystem.h"
#include "UIWireSegmentsSystem.h"
#include "UIWireValidationSystem.h"
#include "WindowSizeComponent.h"
#include "WireTextureSizeComponent.h"
#include "WorldMouseComponent.h"

void xg::SetupWorld(flecs::world& world)
{
    world.emplace<xg::CameraComponent>();
    world.emplace<xg::CameraInputComponent>();
    world.emplace<xg::CogMap>();
    world.emplace<xg::GridAttachmentsComponent>();
    world.emplace<xg::InputComponent>();
    world.emplace<xg::OnStageAddedComponent>();
    world.emplace<xg::OnStageRemovedComponent>();
    world.emplace<xg::RenderSettings>();
    world.emplace<xg::UIDraggingDropComponent>();
    world.emplace<xg::UIDragValidComponent>();
    world.emplace<xg::UIHoverComponent>();
    world.emplace<xg::UIPreviewAddingCogComponent>();
    world.emplace<xg::UIPreviewAddingWireComponent>();
    world.emplace<xg::UIPreviewCreateWireComponent>();
    world.emplace<xg::UIRotateComponent>();
    world.emplace<xg::UISettings>();
    world.emplace<xg::WindowSizeComponent>();
    world.emplace<xg::WorldMouseComponent>();

    world.ensure<xg::GridSizeComponent>().m_Size = glm::ivec2(100, 100);
    world.ensure<xg::WireTextureSizeComponent>().m_Size = glm::ivec2(128, 4);
    world.ensure<xg::command::ListComponent>().m_Commands.resize(500);

    xg::cog::RegisterAll(world.get_mut<xg::CogMap>());

    world.component<xg::CogCreatedComponent>();
    world.component<xg::command::RemovedFromHistoryComponent>();
    world.component<xg::command::ExecuteComponent>();
    world.component<xg::UIUndoComponent>();
}

void xg::UpdateWorld(flecs::world& world, const double time, const float deltaTime)
{
    xg::CameraInputSystem::Update(world, time, deltaTime);
    xg::CameraSystem::Update(world, time, deltaTime);
    xg::UIHoverSystem::Update(world);
    xg::UIDragDropSystem::Update(world);
    xg::UIDragPreviewSystem::Update(world);
    xg::UIDragValidationSystem::Update(world);
    xg::UIPreviewWireSystem::Update(world);
    xg::UIWirePathfindingSystem::Update(world);
    xg::UIWireSegmentsSystem::Update(world);
    xg::UIWireValidationSystem::Update(world);
    xg::command::CreateSystem::Update(world);
    xg::command::ListSystem::Update(world);
    xg::CogSystem::Update(world);
    xg::cog::BatterySystem::Update(world);
    xg::OnStageSystem::Update(world);
    xg::GridAttachmentSystem::Update(world);
}