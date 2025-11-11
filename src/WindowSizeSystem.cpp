#include "WindowSizeSystem.h"

#include <flecs/flecs.h>

#include "Core/GLFWLib.h"
#include "WindowSizeComponent.h"

void xg::WindowSizeSystem::Update(flecs::world& world, GLFWwindow* window)
{
    xg::WindowSizeComponent& windowSize = world.get_mut<xg::WindowSizeComponent>();
    glfwGetWindowSize(window, &windowSize.m_Width, &windowSize.m_Height);
}