#include "OnStageSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "OnStageComponent.h"

void xg::OnStageSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](
        const xg::command::AddCogComponent&,
        const xg::command::EntityComponent& commandEntity,
        const xg::command::ExecuteComponent&)
        {
            commandEntity.m_Entity.add<xg::OnStageComponent>();
            printf("OnStageAdded\n");
        });

    world.defer_end();

    world.defer_begin();
    world.each([&](
        const xg::command::DeleteCogComponent& deleteCog,
        const xg::command::ExecuteComponent&)
        {
            deleteCog.m_Cog.remove<xg::OnStageComponent>();
            printf("OnStageRemoved\n");
        });

    world.defer_end();
}