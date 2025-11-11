#pragma once

namespace flecs
{
    struct world;
}

namespace xg::InputSystem
{
    void UpdateKeyInput(flecs::world& world, int key, int scancode, int action, int mods);
    void UpdateCursorInput(flecs::world& world, double xpos, double ypos);
    void Update(flecs::world& world);
}