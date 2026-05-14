#pragma once

#include <glm/vec3.hpp>
#include <string>

namespace xg
{
	struct RGBColor
	{
		RGBColor();
		RGBColor(std::string v);

		glm::vec3 m_Value = glm::vec3(0.f);

		operator glm::vec3() const
		{
			return m_Value;
		}
	};
}