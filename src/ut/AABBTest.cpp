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
}