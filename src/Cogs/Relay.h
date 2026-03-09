#pragma once

#include "CogResourceId.h"
#include "Cogs/CogPrototype.h"

namespace flecs
{
    struct world;
}

namespace xg::cog
{
    xg::CogResourceId GetRelayResourceId();

    struct Relay final : public xg::CogPrototype
    {
        DECLARE_COG_FUNCTIONS();
    };
}
