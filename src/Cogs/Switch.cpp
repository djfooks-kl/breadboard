#include "Switch.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Switch");
}

xg::CogResourceId xg::cog::GetSwitchResourceId()
{
    return s_ResourceId;
}


xg::CogResourceId xg::cog::Switch::GetResourceId() const
{
    return s_ResourceId;
}

glm::ivec2 xg::cog::Switch::GetSize() const
{
    return glm::ivec2(2, 2);
}