#include "Battery.h"

#include <flecs/flecs.h>

#include "Cogs/BatteryComponent.h"

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Battery");
}

xg::CogResourceId xg::cog::Battery::GetResourceId()
{
    return s_ResourceId;
}

void xg::cog::Battery::Create(flecs::world& world) const
{
    flecs::entity batteryEntity = world.entity();
    batteryEntity.add<xg::cog::BatteryComponent>();
}