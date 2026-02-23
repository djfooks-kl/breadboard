#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec2.hpp>

namespace xg
{
	struct CameraComponent
	{
		glm::vec2 m_Position = { 5.f, 3.f };
		float m_Zoom = 11.f;

		glm::mat4 m_View;
		glm::mat4 m_Projection;
		glm::mat4 m_ViewProjection;
		glm::mat4 m_InvViewProjection;

		float m_Feather = 0.f;
	};
}