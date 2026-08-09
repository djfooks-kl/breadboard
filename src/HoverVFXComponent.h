#pragma once

#include <glm/vec2.hpp>

namespace xg
{
	struct HoverVFXComponent
	{
		bool m_WasHovering = true;
		float m_InDuration = 0.f;
		float m_OutDuration = 0.f;
		glm::vec2 m_InPosition = glm::vec2(0.f);
		glm::vec2 m_OutPosition = glm::vec2(0.f);
	};
}
