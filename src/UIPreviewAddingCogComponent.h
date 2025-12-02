#pragma once

#include <glm/ext/vector_float2.hpp>

#include "Cogs/CogResourceId.h"

namespace xg
{
    struct UIPreviewAddingCogComponent
    {
        xg::CogResourceId m_AddCogId;
        xg::CogResourceId m_HoverCogId;
        glm::vec2 m_PreviewPosition;
    };
}