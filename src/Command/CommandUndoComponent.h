#pragma once

#include <flecs/flecs.h>

namespace xg::command
{
	struct UndoComponent
	{
		flecs::entity m_Undo;
	};
}