#pragma once

#include <glm/ext/matrix_float4x4.hpp>

namespace xc
{
    class Rotation90;
}

namespace xg
{
    glm::vec2 WindowToWorldPosition(
        const glm::mat4& invViewProjection,
        float windowX,
        float windowY,
        int windowWidth,
        int windowHeight);

    // Creates a camera view matrix so that the scene appears to be transformed
    glm::mat4 CameraViewWithTransformedScene(
        const glm::vec2& cameraPosition,
        const glm::vec2& sceneTranslation,
        const xc::Rotation90& sceneRotation);
}