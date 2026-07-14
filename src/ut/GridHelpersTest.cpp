#include "BreadTest.h"

#include "GridHelpers.h"

namespace
{
	std::vector<glm::ivec2> GetPointsForSegments(const std::vector<glm::ivec2>& segments)
	{
		std::vector<glm::ivec2> result;
		auto toArray = [&](const glm::ivec2& p)
		{
			result.push_back(p);
		};
		xg::ForEachSegmentsCellUntil(segments, toArray);
		return result;
	}
}

TEST_CASE("xg::ForEachCellUntil", "[xc::GridHelpers]")
{
	CHECK(GetPointsForSegments({}).size() == 0);

	REQUIRE(GetPointsForSegments({ glm::ivec2(3, 4) }).size() == 1);
	CHECK(GetPointsForSegments({ glm::ivec2(3, 4) })[0] == glm::ivec2(3, 4));

	{
		std::vector<glm::ivec2> segments;
		segments.push_back(glm::ivec2(1, 4));
		segments.push_back(glm::ivec2(4, 4));
		std::vector<glm::ivec2> result = GetPointsForSegments(segments);
		REQUIRE(result.size() == 4);
		CHECK(result[0] == glm::ivec2(1, 4));
		CHECK(result[1] == glm::ivec2(2, 4));
		CHECK(result[2] == glm::ivec2(3, 4));
		CHECK(result[3] == glm::ivec2(4, 4));
	}

	{
		std::vector<glm::ivec2> segments;
		segments.push_back(glm::ivec2(4, 4));
		segments.push_back(glm::ivec2(6, 6));
		std::vector<glm::ivec2> result = GetPointsForSegments(segments);
		REQUIRE(result.size() == 3);
		CHECK(result[0] == glm::ivec2(4, 4));
		CHECK(result[1] == glm::ivec2(5, 5));
		CHECK(result[2] == glm::ivec2(6, 6));
	}

	{
		std::vector<glm::ivec2> segments;
		segments.push_back(glm::ivec2(4, 4));
		segments.push_back(glm::ivec2(6, 6));
		segments.push_back(glm::ivec2(6, 7));
		std::vector<glm::ivec2> result = GetPointsForSegments(segments);
		REQUIRE(result.size() == 4);
		CHECK(result[0] == glm::ivec2(4, 4));
		CHECK(result[1] == glm::ivec2(5, 5));
		CHECK(result[2] == glm::ivec2(6, 6));
		CHECK(result[3] == glm::ivec2(6, 7));
	}
}
