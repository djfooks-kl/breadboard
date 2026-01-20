#include "Command/CommandCreateSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandRemovedFromHistoryComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandToQueueComponent.h"
#include "UIAddCogComponent.h"
#include "UIDeleteCogComponent.h"

void xg::command::CreateSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([](flecs::entity entity, xg::command::ToQueueComponent)
        {
            entity.remove<xg::command::ToQueueComponent>();
        });
    world.defer_end();

    world.defer_begin();
    world.each([](
        flecs::entity entity,
        xg::command::RemovedFromHistoryComponent,
        xg::command::DeleteCogComponent& deleteCog)
        {
            printf("destruct cog\n");
            deleteCog.m_Cog.destruct();
            entity.destruct();
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIAddCogComponent& uiAddCog)
        {
            auto& addCog = entity.ensure<xg::command::AddCogComponent>();
            addCog.m_CogId = uiAddCog.m_CogId;
            addCog.m_Position = uiAddCog.m_Position;
            addCog.m_Rotation = uiAddCog.m_Rotation;

            flecs::entity addEntity = world.entity();
            entity.ensure<xg::command::EntityComponent>().m_Entity = addEntity;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::DeleteCogComponent>().m_Cog = addEntity;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });

    world.each([&](flecs::entity entity, const xg::UIDeleteCogComponent& uiDeleteCog)
        {
            auto& deleteCog = entity.ensure<xg::command::DeleteCogComponent>();
            deleteCog.m_Cog = uiDeleteCog.m_Cog;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::AddCogComponent>();

            undo.ensure<xg::command::EntityComponent>().m_Entity = deleteCog.m_Cog;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });
    world.defer_end();
}