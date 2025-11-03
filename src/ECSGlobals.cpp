#include "ECSGlobals.h"

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
#include "UIAddCogComponent.h"
#include "UIRedoComponent.h"
#include "UIUndoComponent.h"
#include "WindowSizeComponent.h"
#include "WorldMouseComponent.h"

void xg::ECSGlobalsCreate(flecs::world& world)
{
    world.emplace<xg::CameraComponent>();
    world.emplace<xg::CameraInputComponent>();
    world.emplace<xg::CogMap>();
    world.emplace<xg::InputComponent>();
    world.emplace<xg::MouseTrailComponent>();
    world.emplace<xg::WindowSizeComponent>();
    world.emplace<xg::WorldMouseComponent>();
    world.ensure<xg::command::ListComponent>().m_Commands.resize(500);

    xg::cog::RegisterAll(world.get_mut<xg::CogMap>());

    world.component<xg::command::ExecuteComponent>();
    world.component<xg::UIUndoComponent>();
}
