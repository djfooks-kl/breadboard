#include "CameraHelpers.h"

#include <glm/ext/matrix_transform.hpp>

#include "Core/Rotation90.h"

glm::vec2 xg::WindowToWorldPosition(
    const glm::mat4& invViewProjection,
    float windowX,
    float windowY,
    int windowWidth,
    int windowHeight)
{
    const glm::vec4 viewMouse(
        (windowX * 2.f) / windowWidth - 1.f,
        1.f - (windowY * 2.f) / windowHeight,
        0.f,
        1.f);
    const glm::vec4 worldMousePos = invViewProjection * viewMouse;
    return worldMousePos / worldMousePos.w;
}

glm::mat4 xg::CameraViewWithTransformedScene(
    const glm::vec2& cameraPosition,
    const glm::vec2& sceneTranslation,
    const xc::Rotation90& sceneRotation)
{
    const glm::vec2 relativeCameraPos = sceneRotation.ApplyInverse(cameraPosition - sceneTranslation);
    const glm::vec3 cameraPos = glm::vec3(relativeCameraPos, 0.5f);
    const glm::vec3 cameraTarget = glm::vec3(relativeCameraPos, 0.0f);
    const glm::ivec2 rotatedUp = sceneRotation.ApplyInverse(glm::ivec2(0, 1));
    const glm::vec3 cameraUp = glm::vec3(rotatedUp.x, rotatedUp.y, 0.f);

    return glm::lookAt(cameraPos, cameraTarget, cameraUp);
}