#include "BreadTest.h"

#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>

#include "Core/AABB.h"
#include "CogHelpers.h"
#include "RenderSettings.h"

TEST_CASE("xg::GetCogAABB", "[xg::CogHelpers]")
{
    xg::RenderSettings renderSettings;
    renderSettings.m_CogBoxSize = 0.25f;

    xc::AABB cogAABB = xg::GetCogAABB(glm::ivec2(2, 4), xc::ITransform{ .m_Translation = glm::ivec2(1, 2) }, renderSettings);
    CHECK(cogAABB.m_Min == glm::vec2(0.75f, 1.75f));
    CHECK(cogAABB.m_Max == glm::vec2(2.25f, 5.25f));
}
