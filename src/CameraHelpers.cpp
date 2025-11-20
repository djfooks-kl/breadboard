#include "CameraHelpers.h"

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