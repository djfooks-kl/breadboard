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
    world.get_mut<xg::OnStageAddedComponent>().m_Entities.clear();
    world.get_mut<xg::OnStageRemovedComponent>().m_Entities.clear();

    world.defer_begin();
    world.each([&](
        const xg::command::AddCogComponent&,
        const xg::command::EntityComponent& commandEntity,
        const xg::command::ExecuteComponent&)
        {
            commandEntity.m_Entity.add<xg::OnStageComponent>();
            world.get_mut<xg::OnStageAddedComponent>().m_Entities.push_back(commandEntity.m_Entity);
        });

    world.defer_end();

    world.defer_begin();
    world.each([&](
        const xg::command::DeleteCogComponent& deleteCog,
        const xg::command::ExecuteComponent&)
        {
            deleteCog.m_Cog.remove<xg::OnStageComponent>();
            world.get_mut<xg::OnStageRemovedComponent>().m_Entities.push_back(deleteCog.m_Cog);
        });

    world.defer_end();
}