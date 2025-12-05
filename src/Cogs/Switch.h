#pragma once

#include "CogResourceId.h"
#include "Cogs/CogPrototype.h"

namespace flecs
{
    struct world;
}

namespace xg::cog
{
    xg::CogResourceId GetSwitchResourceId();

    struct Switch final : public xg::CogPrototype
    {
        virtual xg::CogResourceId GetResourceId() const;

        virtual glm::ivec2 GetSize() const;
    };
}
