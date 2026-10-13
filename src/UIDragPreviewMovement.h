#pragma once

#include <glm/ext/vector_float2.hpp>

#include "Core/ITransform.h"

namespace xg
{
    struct UIDragPreviewMovement
    {
        // the translation free from the grid
        glm::vec2 m_FreeTranslation = glm::vec2(0.f);
        // the transform when snapped to the grid
        xc::ITransform m_Transform;
    };
}