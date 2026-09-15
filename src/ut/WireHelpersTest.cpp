#include "BreadTest.h"

#include <glm/geometric.hpp>

#include "Core/AABB.h"
#include "Core/OBB.h"
#include "Core/MathConstants.h"
#include "WireHelpers.h"

TEST_CASE("xg::GetWireSegmentOBB", "[xg::WireHelpers]")
{
	{
		xc::OBB oob = xg::GetWireSegmentOBB(glm::ivec2(10, 10), glm::ivec2(10, 10), 1.5f);
		CHECK(oob.m_Center == glm::vec2(10.f, 10.f));
		CHECK(oob.m_HalfExtents == glm::vec2(0.f, 0.f));
		CHECK(oob.m_AxisX == glm::vec2(1.f, 0.f));
		CHECK(oob.m_AxisY == glm::vec2(0.f, 1.f));
	}
	{
		xc::OBB oob = xg::GetWireSegmentOBB(glm::ivec2(10, 10), glm::ivec2(12, 10), 1.5f);
		CHECK(oob.m_Center == glm::vec2(11.f, 10.f));
		CHECK(oob.m_HalfExtents == glm::vec2(1.5f, 1.f));
		CHECK(oob.m_AxisX == glm::vec2(0.f, -1.f));
		CHECK(oob.m_AxisY == glm::vec2(1.f, 0.f));
	}
	{
		xc::OBB oob = xg::GetWireSegmentOBB(glm::ivec2(100, 100), glm::ivec2(100, 95), 1.5f);
		CHECK(oob.m_Center == glm::vec2(100.f, 97.5f));
		CHECK(oob.m_HalfExtents == glm::vec2(1.5f, 2.5f));
		CHECK(oob.m_AxisX == glm::vec2(-1.f, 0.f));
		CHECK(oob.m_AxisY == glm::vec2(0.f, -1.f));
	}
	{
		xc::OBB oob = xg::GetWireSegmentOBB(glm::ivec2(5, 5), glm::ivec2(7, 7), 1.5f);
		CHECK(oob.m_Center == glm::vec2(6.f, 6.f));
		CHECK(oob.m_HalfExtents == glm::vec2(1.5f, 1.f * SQRT_2));
		CHECK(oob.m_AxisX == glm::normalize(glm::vec2(1.f, -1.f)));
		CHECK(oob.m_AxisY == glm::normalize(glm::vec2(1.f, 1.f)));
	}
	{
		xc::OBB oob = xg::GetWireSegmentOBB(glm::ivec2(20, 20), glm::ivec2(17, 17), 0.5f);
		CHECK(oob.m_Center == glm::vec2(18.5f, 18.5f));
		CHECK(oob.m_HalfExtents == glm::vec2(0.5f, 1.5f * SQRT_2));
		CHECK_NEARLY_GLM_EQUAL(oob.m_AxisX, glm::normalize(glm::vec2(-1.f, 1.f)), 0.01f);
		CHECK_NEARLY_GLM_EQUAL(oob.m_AxisY, glm::normalize(glm::vec2(-1.f, -1.f)), 0.01f);
	}
}