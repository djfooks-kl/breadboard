#pragma once

#include <flecs/flecs.h>

namespace xg
{
	struct UIHoverComponent
	{
		flecs::entity m_Cog;
		bool m_Wire = false;
		bool m_Node = false;
	};
}
