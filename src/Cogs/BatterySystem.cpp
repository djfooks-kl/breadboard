#include "BatterySystem.h"

#include <flecs/flecs.h>

#include "CogComponent.h"
#include "CogCreatedComponent.h"
#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandExecuteComponent.h"

void xg::cog::BatterySystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](
        const xg::CogComponent& cog,
        const xg::CogCreatedComponent&)
        {
            printf("Create cog: %s\n", cog.m_CogId.GetName());
        });

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
            printf("Delete cog: %s\n", deleteCog.m_Cog.get<const xg::CogComponent>().m_CogId.GetName());
        });
    world.defer_end();
}