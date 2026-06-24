#include "OnStageSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageComponent.h"
#include "OnStageRemovedComponent.h"

void xg::OnStageSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([](flecs::entity entity, xg::OnStageAddedComponent)
        {
            entity.remove<xg::OnStageAddedComponent>();
        });
    world.each([](flecs::entity entity, xg::OnStageRemovedComponent)
        {
            entity.remove<xg::OnStageRemovedComponent>();
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](
        const xg::command::AddCogComponent&,
        const xg::command::EntityComponent& commandEntity,
        const xg::command::ExecuteComponent&)
        {
            commandEntity.m_Entity.add<xg::OnStageComponent>();
            commandEntity.m_Entity.add<xg::OnStageAddedComponent>();
        });

    world.defer_end();

    world.defer_begin();
    world.each([&](
        const xg::command::DeleteCogComponent& deleteCog,
        const xg::command::ExecuteComponent&)
        {
            deleteCog.m_Cog.remove<xg::OnStageComponent>();
            deleteCog.m_Cog.add<xg::OnStageRemovedComponent>();
        });

    world.defer_end();
}