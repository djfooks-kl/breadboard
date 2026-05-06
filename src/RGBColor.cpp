#include "RGBColor.h"

xg::RGBColor::RGBColor() = default;

xg::RGBColor::RGBColor(std::string hex)
{
	if (hex[0] == '#')
	{
		hex = hex.substr(1);
	}

	int r, g, b;
	sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b);

	m_Value = glm::vec3{
		static_cast<float>(r) / 255.f,
		static_cast<float>(g) / 255.f,
		static_cast<float>(b) / 255.f};
}