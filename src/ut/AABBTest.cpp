#include "BreadTest.h"

#include "Core/AABB.h"

TEST_CASE("Simple AABB tests", "[xc::AABB]")
{
    xc::AABB aabb{ .m_Min = glm::vec2(1.f, 2.f), .m_Max = glm::vec2(5.f, 3.f) };

    CHECK(aabb.Contains(glm::vec2(1.f, 2.f)));
    CHECK(aabb.Contains(glm::vec2(1.f, 3.f)));
    CHECK(aabb.Contains(glm::vec2(2.f, 2.f)));
    CHECK(aabb.Contains(glm::vec2(2.f, 3.f)));
    CHECK(aabb.Contains(glm::vec2(3.f, 2.f)));
    CHECK(aabb.Contains(glm::vec2(3.f, 3.f)));
    CHECK(aabb.Contains(glm::vec2(4.f, 2.f)));
    CHECK(aabb.Contains(glm::vec2(4.f, 3.f)));
    CHECK(aabb.Contains(glm::vec2(5.f, 2.f)));
    CHECK(aabb.Contains(glm::vec2(5.f, 3.f)));

    CHECK(aabb.Contains(glm::vec2(5.01f, 3.f)) == false);
    CHECK(aabb.Contains(glm::vec2(0.99f, 2.f)) == false);
    CHECK(aabb.Contains(glm::vec2(1.f, 4.01f)) == false);
    CHECK(aabb.Contains(glm::vec2(5.f, 0.99f)) == false);
    CHECK(aabb.Contains(glm::vec2(5.01f, 4.f)) == false);

    CHECK(aabb.Overlaps(aabb) == true);
    CHECK(aabb.Overlaps(xc::AABB{ .m_Min = glm::vec2(0.f, 0.f), .m_Max = glm::vec2(5.f, 3.f) }) == true);
    CHECK(aabb.Overlaps(xc::AABB{ .m_Min = glm::vec2(0.f, 0.f), .m_Max = glm::vec2(0.5f, 0.5f) }) == false);
    CHECK(aabb.Overlaps(xc::AABB{ .m_Min = glm::vec2(6.f, 0.f), .m_Max = glm::vec2(6.5f, 0.5f) }) == false);
    CHECK(aabb.Overlaps(xc::AABB{ .m_Min = glm::vec2(0.f, 3.5f), .m_Max = glm::vec2(5.f, 4.f) }) == false);
    CHECK(aabb.Overlaps(xc::AABB{ .m_Min = glm::vec2(0.f, 0.f), .m_Max = glm::vec2(5.f, 1.5f) }) == false);

    CHECK(aabb.FromPoints(glm::vec2(0.5f, 10.f), glm::vec2(5.f, 0.5f)).m_Min == glm::vec2(0.5f, 0.5f));
    CHECK(aabb.FromPoints(glm::vec2(0.5f, 10.f), glm::vec2(5.f, 0.5f)).m_Max == glm::vec2(5.f, 10.f));

    CHECK(aabb.ClampPointInside(glm::vec2(4.f, 2.5f)) == glm::vec2(4.f, 2.5f));
    CHECK(aabb.ClampPointInside(glm::vec2(0.5f, 4.f)) == glm::vec2(1.f, 3.f));
    CHECK(aabb.ClampPointInside(glm::vec2(6.f, -1.f)) == glm::vec2(5.f, 2.f));

    CHECK(aabb.ToCenterExtents().m_Center == glm::vec2(3.f, 2.5f));
    CHECK(aabb.ToCenterExtents().m_HalfExtents == glm::vec2(2.f, 0.5f));
}