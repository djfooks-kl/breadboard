#pragma once

#include <flecs/flecs.h>

namespace xg
{
	struct UIHoverComponent
	{
		flecs::entity m_Cog;
		flecs::entity m_Wire;
		bool m_Node = false;
	};
}
