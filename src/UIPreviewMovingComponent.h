#pragma once

#include <glm/ext/vector_float2.hpp>

namespace xg
{
    struct UIPreviewMovingComponent
    {
        flecs::entity m_Dragging;
        glm::vec2 m_GrabbedPosition = glm::vec2(0.f);
    };
}