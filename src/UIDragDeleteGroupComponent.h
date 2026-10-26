#pragma once

#include "Core/ITransform.h"

#include <flecs/flecs.h>
#include <vector>

namespace xg
{
    struct UIDragDeleteGroupComponent
    {
        std::vector<flecs::entity> m_Entities;
    };
}
