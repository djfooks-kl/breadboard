#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>

#include "Core/ITransform.h"

namespace xc
{
	struct AABB
	{
		bool Contains(const glm::vec2& p) const;

		bool operator==(const xc::AABB& other) const = default;

		// min values are inclusive
		glm::vec2 m_Min;
		// max values are inclusive
		glm::vec2 m_Max;
	};

	std::ostream& operator<<(std::ostream& os, const xc::AABB& obj);
}