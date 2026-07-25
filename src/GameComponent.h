#pragma once

#include <source_location>

namespace xg
{
    struct GameComponent
    {
        std::source_location m_CreatedAt;
    };
}