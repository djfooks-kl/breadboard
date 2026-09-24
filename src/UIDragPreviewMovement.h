#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_int2.hpp>

#include "Cogs/CogResourceId.h"
#include "Core/Rotation90.h"

namespace xg
{
    struct UIDragPreviewMovement
    {
        glm::vec2 m_GrabbedPosition = glm::vec2(0.f);
        glm::vec2 m_Translation = glm::vec2(0.f);
        xc::Rotation90 m_Rotation;
    };
}