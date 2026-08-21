#include "CameraInputSystem.h"

#include <cmath>
#include <flecs/flecs.h>

#include "CameraInputComponent.h"
#include "Core/GLFWLib.h"
#include "MappedInputComponent.h"

void xg::CameraInputSystem::Update(flecs::world& world, double /*time*/, float /*deltaTime*/)
{
    auto& cameraInput = world.get_mut<xg::CameraInputComponent>();
    const auto& input = world.get<xg::MappedInputComponent>();

    cameraInput.m_Position.x =
        (input.m_KeyDown.contains(xg::EMappedInput::MoveLeft) ? -1.f : 0.f) +
         (input.m_KeyDown.contains(xg::EMappedInput::MoveRight) ? 1.f : 0.f);

    cameraInput.m_Position.y =
        (input.m_KeyDown.contains(xg::EMappedInput::MoveDown) ? -1.f : 0.f) +
        (input.m_KeyDown.contains(xg::EMappedInput::MoveUp) ? 1.f : 0.f);

    cameraInput.m_Zoom =
        (input.m_KeyDown.contains(xg::EMappedInput::ZoomIn) ? -1.f : 0.f) +
        (input.m_KeyDown.contains(xg::EMappedInput::ZoomOut) ? 1.f : 0.f);
}
