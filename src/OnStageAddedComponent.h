#pragma once

#include <vector>
#include <flecs/flecs.h>

namespace xg
{
	struct OnStageAddedComponent
	{
		std::vector<flecs::entity> m_Entities;
	};
}
