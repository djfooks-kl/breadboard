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
        glm::ivec2 m_Position;
        xc::Rotation90 m_Rotation;
        glm::vec2 m_PreviewPosition;
    };
}