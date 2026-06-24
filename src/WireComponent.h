#pragma once

#include <glm/ext/vector_int2.hpp>
#include <vector>

namespace xg
{
	struct WireComponent
	{
		std::vector<glm::ivec2> m_Checkpoints;
	};
}
