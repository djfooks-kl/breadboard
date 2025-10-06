#include "ECSGlobals.h"

#include <flecs/flecs.h>

#include "Cogs/CogMap.h"
#include "Cogs/Register.h"

void xg::ECSGlobalsCreate(flecs::world& world)
{
    world.emplace<xg::CogMap>();
    xg::cog::RegisterAll(world.get_mut<xg::CogMap>());
}
