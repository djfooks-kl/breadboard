#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>

namespace xg
{
	struct WorldMouseComponent
	{
		glm::vec2 m_Position = { 0.f, 0.f };
		glm::ivec2 m_Cell = { 0, 0 };
	};
}