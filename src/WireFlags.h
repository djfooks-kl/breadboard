#pragma once

#include "Core/EnumFlags.h"

#define BIT(x) (1u << x)

namespace xg
{
    enum EWireDirection : uint8_t
    {
        None = 0,
        N = BIT(0),
        NE = BIT(1),
        E = BIT(2),
        SE = BIT(3),
        S = BIT(4),
        SW = BIT(5),
        W = BIT(6),
        NW = BIT(7),

        All = 255
    };

    using TWireDirectionFlags = xc::EnumFlags<EWireDirection>;
}
