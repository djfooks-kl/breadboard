#pragma once

#include <glm/vec2.hpp>

namespace xg
{
	struct CameraInputComponent
	{
		glm::vec2 m_Position = { 0.f, 0.f };
		float m_Zoom = 0;
	};
}