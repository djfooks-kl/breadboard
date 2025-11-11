#pragma once

#include "CogResourceId.h"
#include "Cogs/Cog.h"

namespace flecs
{
    struct world;
}

namespace xg::cog
{
    struct Switch : public xg::Cog
    {
        static xg::CogResourceId GetResourceId();
    };
}
