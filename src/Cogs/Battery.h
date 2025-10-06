#pragma once

#include "CogResourceId.h"
#include "Cogs/Cog.h"

namespace flecs
{
    struct world;
}

namespace xg::cog
{
    struct Battery : public xg::Cog
    {
        static xg::CogResourceId GetResourceId();

        void Create(flecs::world&) const override;
    };
}
