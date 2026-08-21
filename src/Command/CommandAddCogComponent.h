#pragma once

#include "Cogs/CogResourceId.h"
#include "Core/ITransform.h"

namespace xg::command
{
    struct AddCogComponent
    {
        xg::CogResourceId m_CogId;
        xc::ITransform m_Transform;
    };
}
