#pragma once

#include <flecs/flecs.h>

namespace xg::command
{
	// Attached to command entities that should be added to the queue
	struct ToQueueComponent
	{
		// another command entity to undo this command
		flecs::entity m_Undo;
	};
}
