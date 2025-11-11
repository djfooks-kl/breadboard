#pragma once

namespace flecs
{
    struct world;
}

namespace xg::MouseTrailSystem
{
    void Update(flecs::world& world, const double time, const float deltaTime);
}