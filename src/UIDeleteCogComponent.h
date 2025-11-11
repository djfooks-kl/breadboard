#pragma once

#include <glm/ext/vector_int2.hpp>

#include "Cogs/CogResourceId.h"

namespace xg
{
    struct UIDeleteCogComponent
    {
        xg::CogResourceId m_CogId;
        glm::ivec2 m_Position;
    };
}