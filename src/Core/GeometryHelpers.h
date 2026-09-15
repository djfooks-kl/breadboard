#pragma once

#include <glm/vec2.hpp>

namespace xc
{
    struct AABB;
    struct AABBCenterExtents;
    struct OBB;

    bool CircleOverlapsAABB(const glm::vec2& circleCenter, const float radius, const xc::AABB& aabb);

    bool AABBOverlapsOBB(const xc::AABBCenterExtents& aabb, const xc::OBB& obb);
}