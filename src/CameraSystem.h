#pragma once

namespace flecs
{
    struct world;
}

namespace xg::CameraSystem
{
    void Update(flecs::world& world, const double time, const float deltaTime);
}