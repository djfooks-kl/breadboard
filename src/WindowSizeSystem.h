#pragma once

struct GLFWwindow;

namespace flecs
{
    struct world;
}

namespace xg::WindowSizeSystem
{
    void Update(flecs::world& world, GLFWwindow* window);
}
