#pragma once

#include <flecs/flecs.h>
#include <vector>

namespace xg::command
{
    struct AddGroupComponent
    {
        std::vector<flecs::entity> m_Entities;
    };
}
