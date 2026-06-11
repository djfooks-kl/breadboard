#include "Core/IAABB.h"

#include <ostream>

xc::IAABB xc::IAABB::FromTransformAndSize(const ITransform& transform, const glm::ivec2& size)
{
	glm::ivec2 p1 = transform.m_Translation;
	glm::ivec2 p2 = transform.Apply(size - glm::ivec2(1, 1));
	glm::ivec2 min(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
	glm::ivec2 max(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
	return xc::IAABB{ .m_Min = min, .m_Max = max };
}

bool xc::IAABB::Contains(const glm::ivec2& p) const
{
	return
		p.x >= m_Min.x &&
		p.x <= m_Max.x &&
		p.y >= m_Min.y &&
		p.y <= m_Max.y;
}

std::ostream& operator<<(std::ostream& os, const xc::IAABB& obj)
{
	os << "{ "
		"min=[" << obj.m_Min.x << ", " << obj.m_Min.y << ", " << "], "
		"max=[" << obj.m_Max.x << ", " << obj.m_Max.y << ", " << "] }";
	return os;
}