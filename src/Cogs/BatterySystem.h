#pragma once

namespace flecs
{
    struct world;
}

namespace xg::cog::battery_system
{
    void Update(flecs::world& world);
}