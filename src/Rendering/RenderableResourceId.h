#pragma once

#include "Core/ResourceId.h"

namespace xg
{
    struct RenderableResourceType
    {
        static const char* GetName() { return "Renderable"; }
    };
    using RenderableResourceId = xc::ResourceId<RenderableResourceType>;
}
