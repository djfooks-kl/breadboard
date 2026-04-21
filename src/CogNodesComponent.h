#pragma once

#include <vector>
#include <glm/ext/vector_int2.hpp>

namespace xg
{
	struct CogNodesComponent
	{
		std::vector<glm::ivec2> m_Nodes;
	};
}
