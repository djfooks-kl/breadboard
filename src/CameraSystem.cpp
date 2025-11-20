#include "CameraSystem.h"

#include <algorithm>
#include <cmath>
#include <flecs/flecs.h>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "CameraHelpers.h"
#include "CameraComponent.h"
#include "CameraInputComponent.h"
#include "InputComponent.h"
#include "WindowSizeComponent.h"
#include "WorldMouseComponent.h"

namespace
{
    constexpr float s_Speed = 1.f;
    constexpr float s_ZoomSpeed = 3.f;

    constexpr float s_ZoomMin = 1.f;
    constexpr float s_ZoomMax = 800.f;
}

void xg::CameraSystem::Update(flecs::world& world, const double /*time*/, const float deltaTime)
{
    auto& camera = world.get_mut<xg::CameraComponent>();
    auto& worldMouse = world.get_mut<xg::WorldMouseComponent>();
    const auto& cameraInput = world.get<xg::CameraInputComponent>();
    const auto& input = world.get<xg::InputComponent>();
    const auto& windowSize = world.get<xg::WindowSizeComponent>();

    camera.m_Zoom *= 1.f + (cameraInput.m_Zoom * s_ZoomSpeed * deltaTime);
    camera.m_Position += cameraInput.m_Position * s_Speed * deltaTime * camera.m_Zoom;

    camera.m_Zoom = std::max(camera.m_Zoom, s_ZoomMin);
    camera.m_Zoom = std::min(camera.m_Zoom, s_ZoomMax);

    const float aspect = (float)windowSize.m_Width / (float)windowSize.m_Height;
    const float orthoHeight = camera.m_Zoom;
    const float orthoWidth  = orthoHeight * aspect;

    const glm::vec3 cameraPos = glm::vec3(camera.m_Position, 0.5f);
    const glm::vec3 cameraTarget = glm::vec3(camera.m_Position, 0.0f);
    const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    camera.m_View = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    camera.m_Projection = glm::ortho(
        -orthoWidth * 0.5f, orthoWidth * 0.5f,
        -orthoHeight * 0.5f, orthoHeight * 0.5f,
        -1.0f, 1.0f);

    camera.m_ViewProjection = camera.m_Projection * camera.m_View;
    camera.m_InvViewProjection = glm::inverse(camera.m_ViewProjection); // TODO try transpose

    worldMouse.m_Position = WindowToWorldPosition(
        camera.m_InvViewProjection,
        input.m_WindowMouse.x,
        input.m_WindowMouse.y,
        windowSize.m_Width,
        windowSize.m_Height);

    camera.m_Feather = std::max(
        (orthoWidth) / windowSize.m_Width,
        (orthoHeight) / windowSize.m_Height) * 2.f;
}
