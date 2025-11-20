#include "Cogs/Register.h"

#include "Cogs/Battery.h"
#include "Cogs/CogMap.h"
#include "Cogs/Relay.h"
#include "Cogs/Switch.h"

void xg::cog::RegisterAll(xg::CogMap& map)
{
    map.Register<xg::cog::Battery>();
    map.Register<xg::cog::Relay>();
    map.Register<xg::cog::Switch>();
}
