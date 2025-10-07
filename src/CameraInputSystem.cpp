#include "CameraInputSystem.h"

#include <cmath>
#include <flecs/flecs.h>

#include "CameraInputComponent.h"
#include "Core/GLFWLib.h"
#include "InputComponent.h"

void camera_input_system::Update(flecs::world& world, double /*time*/, float /*deltaTime*/)
{
    auto& cameraInput = world.get_mut<xg::CameraInputComponent>();
    const auto& input = world.get<xg::InputComponent>();

    cameraInput.m_Position.x =
        (input.m_KeyDown.contains(GLFW_KEY_A) ? -1.f : 0.f) +
         (input.m_KeyDown.contains(GLFW_KEY_D) ? 1.f : 0.f);

    cameraInput.m_Position.y =
        (input.m_KeyDown.contains(GLFW_KEY_S) ? -1.f : 0.f) +
        (input.m_KeyDown.contains(GLFW_KEY_W) ? 1.f : 0.f);

    cameraInput.m_Zoom =
        (input.m_KeyDown.contains(GLFW_KEY_E) ? -1.f : 0.f) +
        (input.m_KeyDown.contains(GLFW_KEY_Q) ? 1.f : 0.f);
}
