#pragma once

namespace flecs
{
    struct world;
}

namespace xg::CameraInputSystem
{
    void Update(flecs::world& world, double time, float deltaTime);
}