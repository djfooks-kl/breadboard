#include <catch2/catch_test_macros.hpp>
#include <unordered_set>
#include <flecs/flecs.h>

#include "Cogs/CogMap.h"
#include "Cogs/Battery.h"
#include "Cogs/BatteryComponent.h"
#include "Cogs/Register.h"

TEST_CASE("Cog create tests", "[xg::cog]")
{
    flecs::world world;

    xg::CogMap cogMap;
    xg::cog::RegisterAll(cogMap);

    const xg::Cog* cogPrototype = cogMap.Get(xg::cog::Battery::GetResourceId());
    REQUIRE(cogPrototype);
    cogPrototype->Create(world);

    CHECK(world.count<xg::cog::BatteryComponent>() == 1);
}