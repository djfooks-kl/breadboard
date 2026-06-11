#include "Core/AABB.h"

#include <ostream>

bool xc::AABB::Contains(const glm::vec2& p) const
{
	return
		p.x >= m_Min.x &&
		p.x <= m_Max.x &&
		p.y >= m_Min.y &&
		p.y <= m_Max.y;
}

std::ostream& operator<<(std::ostream& os, const xc::AABB& obj)
{
	os << "{ "
		"min=[" << obj.m_Min.x << ", " << obj.m_Min.y << ", " << "], "
		"max=[" << obj.m_Max.x << ", " << obj.m_Max.y << ", " << "] }";
	return os;
}