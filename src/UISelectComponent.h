#pragma once

#include <flecs/flecs.h>

namespace xg
{
    struct UISelectComponent
    {
        flecs::entity m_SelectEntity;
        bool m_Clear = false;
    };
}
