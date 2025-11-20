#pragma once

#include <glm/fwd.hpp>

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

        virtual xg::CogResourceId GetResourceId() const = 0;

        virtual glm::ivec2 GetSize() const = 0;
    };
}
