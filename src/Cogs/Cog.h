#pragma once

#include "CogResourceId.h"

namespace flecs
{
    struct world;
}

namespace xg
{
    struct Cog
    {
        virtual ~Cog();

        virtual void Create(flecs::world&) const = 0;
    };
}
