#include "Cogs/Register.h"

#include "Cogs/Battery.h"
#include "Cogs/Switch.h"
#include "Cogs/CogMap.h"

void xg::cog::RegisterAll(xg::CogMap& map)
{
    map.Register<xg::cog::Battery>();
    map.Register<xg::cog::Switch>();
}
