#include "WindowSizeSystem.h"

#include <flecs/flecs.h>

#include "Core/GLFWLib.h"
#include "WindowSizeComponent.h"

void window_size_system::Update(flecs::world& world, GLFWwindow* window)
{
    xg::WindowSizeComponent& windowSize = world.get_mut<xg::WindowSizeComponent>();
    glfwGetWindowSize(window, &windowSize.m_Width, &windowSize.m_Height);
}