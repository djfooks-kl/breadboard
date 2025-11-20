#include "Battery.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Battery");
}

xg::CogResourceId xg::cog::GetBatteryResourceId()
{
    return s_ResourceId;
}


xg::CogResourceId xg::cog::Battery::GetResourceId() const
{
    return s_ResourceId;
}

glm::ivec2 xg::cog::Battery::GetSize() const
{
    return glm::ivec2(1, 2);
}