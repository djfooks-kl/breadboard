#pragma once

#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_float2.hpp>

#include "Cogs/CogResourceId.h"
#include "Core/Rotation90.h"

namespace xg
{
    struct UIDragPreviewComponent
    {
        xg::CogResourceId m_CogId;
        xc::Rotation90 m_Rotation;
        glm::ivec2 m_Position;
        glm::vec2 m_PreviewPosition;
    };
}