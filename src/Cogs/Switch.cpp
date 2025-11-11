#include "Switch.h"

#include <flecs/flecs.h>

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Switch");
}

xg::CogResourceId xg::cog::Switch::GetResourceId()
{
    return s_ResourceId;
}
