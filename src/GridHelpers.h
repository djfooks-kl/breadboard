#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace xg
{
    glm::ivec2 SnapToGrid(const glm::vec2& position);

    glm::ivec2 GetIDirection(const glm::ivec2& from, const glm::ivec2& to);

    // Allows diagonal movement to count as 1 unit of distance
    int ChebyshevDistance(const glm::ivec2& from, const glm::ivec2& to);

    // Iterate all of the cells inbetween each pair of points
    template<typename TCallback>
    void ForEachSegmentsCellUntil(const std::vector<glm::ivec2>& points, TCallback callback)
    {
        if (points.empty())
            return;

        if (points.size() == 1)
        {
            callback(points[0]);
            return;
        }

        for (int i = 1; i < points.size(); ++i)
        {
            const glm::ivec2& prev = points[i - 1];
            const glm::ivec2& next = points[i];
            const glm::ivec2 dir = xg::GetIDirection(prev, next);
            const int distance = xg::ChebyshevDistance(prev, next);
            for (int j = (i == 1 ? 0 : 1); j <= distance; ++j)
            {
                const glm::ivec2 p = prev + dir * j;
                callback(p);
            }
        }
    }
}