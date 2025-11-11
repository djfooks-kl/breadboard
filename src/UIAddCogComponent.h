#pragma once

#include <glm/ext/vector_int2.hpp>

#include "Cogs/CogResourceId.h"
#include "Core/Rotation90.h"

namespace xg
{
    struct UIAddCogComponent
    {
        xg::CogResourceId m_CogId;
        glm::ivec2 m_Position;
        xc::Rotation90 m_Rotation;
    };
}