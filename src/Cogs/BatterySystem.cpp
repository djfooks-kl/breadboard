#include "BatterySystem.h"

#include <flecs/flecs.h>

#include "Command/CommandExecuteComponent.h"
#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"

void xg::cog::BatterySystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](
        const xg::command::AddCogComponent& addCog,
        const xg::command::ExecuteComponent&)
        {
            printf("Add cog: %s\n", addCog.m_CogId.GetName());
        });

    world.each([&](
        const xg::command::DeleteCogComponent& deleteCog,
        const xg::command::ExecuteComponent&)
        {
            printf("Delete cog: %s\n", deleteCog.m_CogId.GetName());
        });
    world.defer_end();
}