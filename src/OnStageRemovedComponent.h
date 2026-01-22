#pragma once

#include <vector>
#include <flecs/flecs.h>

namespace xg
{
	struct OnStageRemovedComponent
	{
		std::vector<flecs::entity> m_Entities;
	};
}
