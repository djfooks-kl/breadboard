#pragma once

#include <glm/ext/vector_int2.hpp>

#include "Core/Rotation90.h"

namespace xc
{
	class ITransform
	{
	public:
		// Apply the transform to a vector in local space
		// Returns the world space position
		[[nodiscard]] glm::ivec2 Apply(const glm::ivec2& localV) const;

		// Apply the inverse transform to convert a vector to world space
		// Returns the local space position
		[[nodiscard]] glm::ivec2 ApplyInverse(const glm::ivec2& worldV) const;

		// Apply the transform to a vector in local space and create a new translation with that translation
		// and the original rotation
		[[nodiscard]] xc::ITransform WithLocalTranslation(const glm::ivec2& v) const;

		bool operator==(const xc::ITransform& other) const = default;

		glm::ivec2 m_Translation = glm::ivec2(0);
		xc::Rotation90 m_Rotation;
	};

	std::ostream& operator<<(std::ostream& os, const xc::ITransform& obj);
}