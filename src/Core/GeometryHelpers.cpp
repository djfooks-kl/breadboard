#include "Core/GeometryHelpers.h"

#include "Core/AABB.h"
#include "Core/OBB.h"

bool xc::CircleOverlapsAABB(const glm::vec2& circleCenter, const float radius, const xc::AABB& aabb)
{
    const glm::vec2 closest = aabb.ClampPointInside(circleCenter);
    const glm::vec2 v = circleCenter - closest;
    const float distanceSq = glm::dot(v, v);
    return distanceSq < radius * radius;
}

bool xc::AABBOverlapsOBB(const xc::AABBCenterExtents& aabb, const xc::OBB& obb)
{
    const std::array<glm::vec2, 4> axes{
        glm::vec2(1.f, 0.f),
        glm::vec2(0.f, 1.f),
        obb.m_AxisX,
        obb.m_AxisY,
    };

    for (const glm::vec2& axis : axes)
    {
        const float aabbAxisRadius =
            aabb.m_HalfExtents.x * std::abs(axis.x) +
            aabb.m_HalfExtents.y * std::abs(axis.y);
        const float obbAxisRadius =
            obb.m_HalfExtents.x * std::abs(glm::dot(axis, obb.m_AxisX)) +
            obb.m_HalfExtents.y * std::abs(glm::dot(axis, obb.m_AxisY));

        const float distance = std::abs(glm::dot(obb.m_Center - aabb.m_Center, axis));
        if (distance > aabbAxisRadius + obbAxisRadius)
            return false;
    }
    return true;
}