#include "BreadEntityWorld.h"

#include <flecs/flecs.h>

#include "CameraComponent.h"
#include "CameraInputComponent.h"
#include "Cogs/CogMap.h"
#include "Cogs/Register.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandListComponent.h"
#include "GlobalComponent.h"
#include "InputComponent.h"
#include "MouseTrailComponent.h"
#include "UIRedoComponent.h"
#include "UIUndoComponent.h"
#include "WindowSizeComponent.h"
#include "WorldMouseComponent.h"

#include "CameraInputSystem.h"
#include "CameraSystem.h"
#include "Cogs/BatterySystem.h"
#include "Command/CommandCreateSystem.h"
#include "Command/CommandListSystem.h"
#include "InputSystem.h"
#include "MouseTrailSystem.h"
#include "UIDragDropSystem.h"
#include "UIDragPreviewSystem.h"
#include "UIRotateComponent.h"

void xg::SetupWorld(flecs::world& world)
{
    world.emplace<xg::CameraComponent>();
    world.emplace<xg::CameraInputComponent>();
    world.emplace<xg::CogMap>();
    world.emplace<xg::InputComponent>();
    world.emplace<xg::MouseTrailComponent>();
    world.emplace<xg::UIRotateComponent>();
    world.emplace<xg::WindowSizeComponent>();
    world.emplace<xg::WorldMouseComponent>();

    world.ensure<xg::command::ListComponent>().m_Commands.resize(500);

    xg::cog::RegisterAll(world.get_mut<xg::CogMap>());

    world.component<xg::command::ExecuteComponent>();
    world.component<xg::UIUndoComponent>();
}

void xg::UpdateWorld(flecs::world& world, const double time, const float deltaTime)
{
    xg::CameraInputSystem::Update(world, time, deltaTime);
    xg::CameraSystem::Update(world, time, deltaTime);
    xg::MouseTrailSystem::Update(world, time, deltaTime);
    xg::UIDragPreviewSystem::Update(world);
    xg::UIDragDropSystem::Update(world);
    xg::command::CreateSystem::Update(world);
    xg::command::ListSystem::Update(world);
    xg::cog::BatterySystem::Update(world);
}