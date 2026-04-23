#pragma once

#include "Cogs/CogResourceId.h"
#include "Core/ITransform.h"

namespace xg
{
    struct UIAddCogComponent
    {
        xg::CogResourceId m_CogId;
        xc::ITransform m_Transform;
    };
}