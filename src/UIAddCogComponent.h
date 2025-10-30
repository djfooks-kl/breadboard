#pragma once

#include "Cogs/CogResourceId.h"
#include "Core/Rotation90.h"

namespace xg
{
    struct UIAddCogComponent
    {
        xg::CogResourceId m_CogId;
        xc::Rotation90 m_Rotation;
    };
}