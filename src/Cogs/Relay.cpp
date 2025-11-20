#include "Relay.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Relay");
}

xg::CogResourceId xg::cog::GetRelayResourceId()
{
    return s_ResourceId;
}


xg::CogResourceId xg::cog::Relay::GetResourceId() const
{
    return s_ResourceId;
}

glm::ivec2 xg::cog::Relay::GetSize() const
{
    return glm::ivec2(1, 3);
}