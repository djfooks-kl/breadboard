#pragma once

#include "Core/ResourceId.h"

namespace xg
{
    struct ShaderProgramResourceType
    {
        static const char* GetName() { return "ShaderProgram"; }
    };
    using ShaderProgramResourceId = xc::ResourceId<ShaderProgramResourceType>;
}
