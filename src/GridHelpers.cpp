#include "GridHelpers.h"

#include <algorithm>
#include <cmath>

namespace
{
    int GetSign(int v)
    {
        return v > 0 ? 1 : (v < 0 ? -1 : 0);
    }
}

glm::ivec2 xg::SnapToGrid(const glm::vec2& position)
{
    return glm::ivec2(std::round(position.x), std::round(position.y));
}

glm::ivec2 xg::GetIDirection(const glm::ivec2& from, const glm::ivec2& to)
{
    return glm::ivec2(GetSign(to.x - from.x), GetSign(to.y - from.y));
}

int xg::ChebyshevDistance(const glm::ivec2& from, const glm::ivec2& to)
{
    return std::max(std::abs(to.x - from.x), std::abs(to.y - from.y));
}