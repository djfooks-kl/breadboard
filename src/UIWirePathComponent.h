#pragma once

#include <glm/ext/vector_int2.hpp>
#include <vector>

namespace xg
{
    struct UIWirePathComponent
    {
        glm::ivec2 m_StartDirection = glm::ivec2(0, 0);
        std::vector<glm::ivec2> m_Path;
    };
}