#pragma once

#include <glm/vec2.hpp>

namespace xc
{
    struct OBB
    {
        glm::vec2 m_Center;
        glm::vec2 m_HalfExtents;
        glm::vec2 m_AxisX;
        glm::vec2 m_AxisY;
    };
}