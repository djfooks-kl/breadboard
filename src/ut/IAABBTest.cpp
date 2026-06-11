#include "BreadTest.h"

#include <algorithm>
#include <vector>

#include "Core/IAABB.h"

TEST_CASE("Simple IAABB tests", "[xc::IAABB]")
{
    xc::IAABB aabb{ .m_Min = glm::ivec2(1, 2), .m_Max = glm::ivec2(5, 3) };

    std::vector<glm::ivec2> results;
    aabb.ForEachCellUntil([&](const glm::ivec2& p)
    {
        results.push_back(p);
        return false;
    });
    REQUIRE(results.size() == 10);
    CHECK(std::ranges::find(results, glm::ivec2(1, 2)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(1, 3)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(2, 2)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(2, 3)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(3, 2)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(3, 3)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(4, 2)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(4, 3)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(5, 2)) != std::ranges::end(results));
    CHECK(std::ranges::find(results, glm::ivec2(5, 3)) != std::ranges::end(results));

    CHECK(aabb.Contains(glm::ivec2(1, 2)));
    CHECK(aabb.Contains(glm::ivec2(1, 3)));
    CHECK(aabb.Contains(glm::ivec2(2, 2)));
    CHECK(aabb.Contains(glm::ivec2(2, 3)));
    CHECK(aabb.Contains(glm::ivec2(3, 2)));
    CHECK(aabb.Contains(glm::ivec2(3, 3)));
    CHECK(aabb.Contains(glm::ivec2(4, 2)));
    CHECK(aabb.Contains(glm::ivec2(4, 3)));
    CHECK(aabb.Contains(glm::ivec2(5, 2)));
    CHECK(aabb.Contains(glm::ivec2(5, 3)));

    CHECK(aabb.Contains(glm::ivec2(6, 3)) == false);
    CHECK(aabb.Contains(glm::ivec2(0, 2)) == false);
    CHECK(aabb.Contains(glm::ivec2(1, 4)) == false);
    CHECK(aabb.Contains(glm::ivec2(5, 1)) == false);
    CHECK(aabb.Contains(glm::ivec2(5, 4)) == false);

    {
        xc::ITransform transform{ .m_Translation = glm::ivec2(1, 1) };
        xc::IAABB result = xc::IAABB::FromTransformAndSize(transform, glm::ivec2(1, 1));
        CHECK(result.m_Min == glm::ivec2(1, 1));
        CHECK(result.m_Max == glm::ivec2(1, 1));
    }

    {
        xc::ITransform transform{ .m_Translation = glm::ivec2(2, 2) };
        xc::IAABB result = xc::IAABB::FromTransformAndSize(transform, glm::ivec2(2, 3));
        CHECK(result.m_Min == glm::ivec2(2, 2));
        CHECK(result.m_Max == glm::ivec2(3, 4));
    }

    {
        xc::ITransform transform{ .m_Translation = glm::ivec2(2, 2), .m_Rotation = xc::Rotation90(1) };
        xc::IAABB result = xc::IAABB::FromTransformAndSize(transform, glm::ivec2(2, 3));
        CHECK(result.m_Min == glm::ivec2(2, 1));
        CHECK(result.m_Max == glm::ivec2(4, 2));
    }

    {
        xc::ITransform transform{ .m_Translation = glm::ivec2(2, 2), .m_Rotation = xc::Rotation90(2) };
        xc::IAABB result = xc::IAABB::FromTransformAndSize(transform, glm::ivec2(2, 3));
        CHECK(result.m_Min == glm::ivec2(1, 0));
        CHECK(result.m_Max == glm::ivec2(2, 2));
    }
}