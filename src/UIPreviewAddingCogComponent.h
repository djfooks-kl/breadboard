#pragma once

#include <glm/ext/vector_float2.hpp>

#include "Cogs/CogResourceId.h"

namespace xg
{
    struct UIPreviewAddingCogComponent
    {
        xg::CogResourceId m_CogId;
        glm::vec2 m_PreviewPosition;
        bool m_Hovering = false;
    };
}