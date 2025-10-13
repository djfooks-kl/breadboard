#include "ECSGlobals.h"

#include <flecs/flecs.h>

#include "CameraComponent.h"
#include "CameraInputComponent.h"
#include "Cogs/CogMap.h"
#include "Cogs/Register.h"
#include "GlobalComponent.h"
#include "InputComponent.h"
#include "MouseTrailComponent.h"
#include "WindowSizeComponent.h"
#include "UIAddCogComponent.h"

#include "Command/CommandExecuteComponent.h"

void xg::ECSGlobalsCreate(flecs::world& world)
{
    world.emplace<xg::CameraComponent>();
    world.emplace<xg::CameraInputComponent>();
    world.emplace<xg::CogMap>();
    world.emplace<xg::InputComponent>();
    world.emplace<xg::MouseTrailComponent>();
    world.emplace<xg::UIAddCogComponent>();
    world.emplace<xg::WindowSizeComponent>();

    xg::cog::RegisterAll(world.get_mut<xg::CogMap>());

    world.component<xg::command::ExecuteComponent>();
}
