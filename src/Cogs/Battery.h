#pragma once

#include "CogResourceId.h"
#include "Cogs/CogPrototype.h"

namespace flecs
{
    struct world;
}

namespace xg::cog
{
    xg::CogResourceId GetBatteryResourceId();

    struct Battery final : public xg::CogPrototype
    {
        DECLARE_COG_FUNCTIONS();
    };
}
