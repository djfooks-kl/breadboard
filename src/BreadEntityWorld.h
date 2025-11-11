#pragma once

namespace flecs
{
    struct world;
}

namespace xg
{
    void SetupWorld(flecs::world& world);
    void UpdateWorld(flecs::world& world, const double time, const float deltaTime);
}