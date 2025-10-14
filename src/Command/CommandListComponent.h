#pragma once

#include <vector>
#include <flecs/flecs.h>

namespace xg::command
{
	struct ListComponent
	{
		std::vector<flecs::entity> m_Commands;
		int m_HeadIndex = 0;
		int m_Count = 0;
		int m_UndoHeadIndex = 0;
		int m_UndoCount = 0;
	};
}