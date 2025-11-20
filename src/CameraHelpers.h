#pragma once

#include <glm/ext/matrix_float4x4.hpp>

namespace xg
{
	glm::vec2 WindowToWorldPosition(
		const glm::mat4& invViewProjection,
		float windowX,
		float windowY,
		int windowWidth,
		int windowHeight);
}