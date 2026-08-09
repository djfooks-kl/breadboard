#pragma once

namespace flecs
{
    struct world;
}

namespace xg::HoverVFXSystem
{
    void Update(flecs::world& world, float deltaTime);
}