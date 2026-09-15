#pragma once

#include <flecs/flecs.h>
#include <vector>

namespace xg
{
    struct SelectionBoxComponent
    {
        std::vector<flecs::entity> m_Entities;
    };
}
