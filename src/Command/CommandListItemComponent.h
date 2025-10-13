#pragma once

#include <flecs/flecs.h>

namespace xg::command
{
	struct ListItemComponent
	{
		flecs::entity m_Previous;
		flecs::entity m_Undo;
		flecs::entity m_Next;
	};
}