#pragma once

#include <glm/vec2.hpp>
#include <unordered_set>

namespace xg
{
	struct InputComponent
	{
		std::unordered_set<int> m_KeyDown;
		std::unordered_set<int> m_KeyPress;
		std::unordered_set<int> m_KeyRelease;

		glm::vec2 m_WindowMouse = { 0.f, 0.f };
	};
}