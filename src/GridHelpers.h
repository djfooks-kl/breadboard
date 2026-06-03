#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>

namespace xg
{
    glm::ivec2 SnapToGrid(const glm::vec2& position);

    glm::ivec2 GetIDirection(const glm::ivec2& from, const glm::ivec2& to);

    /// brief Allows diagonal movement to count as 1 unit of distance
    int ChebyshevDistance(const glm::ivec2& from, const glm::ivec2& to);
}