#pragma once

#include <flecs/flecs.h>

namespace xg::command
{
	struct ListComponent
	{
		flecs::entity m_Head;
		flecs::entity m_Tail;
		flecs::entity m_UndoHead;
	};
}