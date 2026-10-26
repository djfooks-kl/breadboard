#pragma once

#include "Core/ITransform.h"

#include <flecs/flecs.h>
#include <vector>

namespace xg::command
{
    struct MoveGroupComponent
    {
        std::vector<flecs::entity> m_Entities;
        xc::ITransform m_Transform;
    };
}
