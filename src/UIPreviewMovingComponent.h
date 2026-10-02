#pragma once

#include <glm/ext/vector_float2.hpp>

namespace xg
{
    struct UIPreviewMovingComponent
    {
        bool m_Dragging = false;
        glm::vec2 m_GrabbedPosition = glm::vec2(0.f);
    };
}