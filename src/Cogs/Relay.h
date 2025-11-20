#pragma once

#include "CogResourceId.h"
#include "Cogs/Cog.h"

namespace flecs
{
    struct world;
}

namespace xg::cog
{
    xg::CogResourceId GetRelayResourceId();

    struct Relay final : public xg::Cog
    {
        virtual xg::CogResourceId GetResourceId() const;

        virtual glm::ivec2 GetSize() const;
    };
}
