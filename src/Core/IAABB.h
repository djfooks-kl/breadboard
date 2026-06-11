#pragma once

#include <glm/ext/vector_int2.hpp>

#include "Core/ITransform.h"

namespace xc
{
	struct IAABB
	{
		// Construct an IAABB by applying a transform to a size
		static [[nodiscard]] xc::IAABB FromTransformAndSize(const ITransform& transform, const glm::ivec2& size);

		bool operator==(const xc::IAABB& other) const = default;

		bool Contains(const glm::ivec2& p) const;

		template<typename TFn>
		void ForEachCellUntil(TFn fn)
		{
			for (int x = m_Min.x; x <= m_Max.x; ++x)
			{
				for (int y = m_Min.y; y <= m_Max.y; ++y)
				{
					if (fn(glm::ivec2(x, y)))
					{
						return;
					}
				}
			}
		}

		// min values are inclusive
		glm::ivec2 m_Min;
		// max values are inclusive
		glm::ivec2 m_Max;
	};

	std::ostream& operator<<(std::ostream& os, const xc::IAABB& obj);
}