#include "Core/AABB.h"

#include <ostream>

xc::AABB xc::AABB::FromPoints(const glm::vec2& p1, const glm::vec2& p2)
{
    return { .m_Min = glm::min(p1, p2), .m_Max = glm::max(p1, p2) };
}

bool xc::AABB::Contains(const glm::vec2& p) const
{
    return
        p.x >= m_Min.x &&
        p.x <= m_Max.x &&
        p.y >= m_Min.y &&
        p.y <= m_Max.y;
}

bool xc::AABB::Overlaps(const xc::AABB& other) const
{
    return
        m_Min.x <= other.m_Max.x &&
        m_Max.x >= other.m_Min.x &&
        m_Min.y <= other.m_Max.y &&
        m_Max.y >= other.m_Min.y;
}

glm::vec2 xc::AABB::ClampPointInside(const glm::vec2& p) const
{
    return glm::clamp(p, m_Min, m_Max);
}

xc::AABBCenterExtents xc::AABB::ToCenterExtents() const
{
    return xc::AABBCenterExtents{
        .m_Center = (m_Min + m_Max) * 0.5f,
        .m_HalfExtents = (m_Max - m_Min) * 0.5f
    };
}

std::ostream& operator<<(std::ostream& os, const xc::AABB& obj)
{
    os << "{ "
        "min=[" << obj.m_Min.x << ", " << obj.m_Min.y << ", " << "], "
        "max=[" << obj.m_Max.x << ", " << obj.m_Max.y << ", " << "] }";
    return os;
}