#include "BreadTest.h"

#include "Core/AABB.h"
#include "Core/OBB.h"
#include "Core/GeometryHelpers.h"
#include "Core/MathConstants.h"

TEST_CASE("xc::CircleOverlapsAABB", "[xc::GeometryHelpers]")
{
    xc::AABB aabb{ .m_Min = glm::vec2(1.f, 2.f), .m_Max = glm::vec2(5.f, 3.f) };

    CHECK(xc::CircleOverlapsAABB(glm::vec2(2.f, 2.5f), 0.5f, aabb) == true);
    CHECK(xc::CircleOverlapsAABB(glm::vec2(0.f, 2.f), 0.99f, aabb) == false);
    CHECK(xc::CircleOverlapsAABB(glm::vec2(0.f, 2.f), 1.01f, aabb) == true);
    CHECK(xc::CircleOverlapsAABB(glm::vec2(6.f, 4.f), SQRT_2 + 0.01f, aabb) == true);
    CHECK(xc::CircleOverlapsAABB(glm::vec2(6.f, 4.f), SQRT_2 - 0.01f, aabb) == false);
}

TEST_CASE("xc::AABBOverlapsOBB", "[xc::GeometryHelpers]")
{
    // aabb from [1,2] to [5,3]
    xc::AABBCenterExtents aabb{ .m_Center = glm::vec2(3.f, 2.5f), .m_HalfExtents = glm::vec2(2.f, 0.5f) };

    xc::OBB obb{
        .m_Center = glm::vec2(5.f, 3.f),
        .m_HalfExtents = glm::vec2(1.f, 2.f),
        .m_AxisX = glm::vec2(1.f, 0.f),
        .m_AxisY = glm::vec2(0.f, 1.f)
    };

    CHECK(xc::AABBOverlapsOBB(aabb, obb) == true);

    obb = xc::OBB{
        .m_Center = glm::vec2(11.f, 7.f),
        .m_HalfExtents = glm::vec2(1.f, 2.f),
        .m_AxisX = glm::vec2(1.f, 0.f),
        .m_AxisY = glm::vec2(0.f, 1.f)
    };
    CHECK(xc::AABBOverlapsOBB(aabb, obb) == false);

    obb = xc::OBB{
        .m_Center = glm::vec2(1.f, 1.5f),
        .m_HalfExtents = glm::vec2(10.f, 0.2f),
        .m_AxisX = glm::normalize(glm::vec2(1.f, -1.f)),
        .m_AxisY = glm::normalize(glm::vec2(1.f, 1.f))
    };
    CHECK(xc::AABBOverlapsOBB(aabb, obb) == false);

    obb = xc::OBB{
        .m_Center = glm::vec2(1.f, 1.5f),
        .m_HalfExtents = glm::vec2(10.f, 0.5f),
        .m_AxisX = glm::normalize(glm::vec2(1.f, -1.f)),
        .m_AxisY = glm::normalize(glm::vec2(1.f, 1.f))
    };
    CHECK(xc::AABBOverlapsOBB(aabb, obb) == true);
}