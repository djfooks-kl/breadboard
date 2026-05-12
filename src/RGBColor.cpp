#include "RGBColor.h"

#include <iostream>
#include <string>

xg::RGBColor::RGBColor() = default;

xg::RGBColor::RGBColor(std::string hex)
{
	if (hex[0] == '#')
	{
		hex = hex.substr(1);
	}

	const int r = std::stoi(hex.substr(0, 2), nullptr, 16);
	const int g = std::stoi(hex.substr(2, 2), nullptr, 16);
	const int b = std::stoi(hex.substr(4, 2), nullptr, 16);

	m_Value = glm::vec3{
		static_cast<float>(r) / 255.f,
		static_cast<float>(g) / 255.f,
		static_cast<float>(b) / 255.f};
}