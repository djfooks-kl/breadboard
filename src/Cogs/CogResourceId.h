#pragma once

#include "Core/ResourceId.h"

namespace xg
{
    struct CogResourceType
    {
        static const char* GetName() { return "Cog"; }
    };
    using CogResourceId = xc::ResourceId<CogResourceType>;
}
