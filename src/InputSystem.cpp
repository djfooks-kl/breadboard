#include "InputSystem.h"

#include <flecs/flecs.h>

#include "Core/GLFWLib.h"
#include "InputComponent.h"

void input_system::UpdateKeyInput(flecs::world& world, int key, int /*scancode*/, int action, int /*mods*/)
{
    world.query_builder<xg::InputComponent>()
        .each([&](xg::InputComponent& input)
    {
        if (action == GLFW_PRESS)
        {
            input.m_KeyDown.insert(key);
            input.m_KeyPress.insert(key);
        }
        else if (action == GLFW_RELEASE)
        {
            input.m_KeyDown.erase(key);
            input.m_KeyRelease.insert(key);
        }
    });
}

void input_system::UpdateCursorInput(flecs::world& world, double xpos, double ypos)
{
    world.query_builder<xg::InputComponent>()
        .each([&](xg::InputComponent& input)
    {
        input.m_WindowMouse = { xpos, ypos };
    });
}

void input_system::Update(flecs::world& world)
{
    world.query_builder<xg::InputComponent>()
        .each([&](xg::InputComponent& input)
    {
        input.m_KeyPress.clear();
        input.m_KeyRelease.clear();
    });
}