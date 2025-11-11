#include "Command/CommandCreateSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
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
    world.each([&](flecs::entity entity, const xg::UIAddCogComponent& uiAddCog)
        {
            auto& addCog = entity.ensure<xg::command::AddCogComponent>();
            addCog.m_CogId = uiAddCog.m_CogId;
            addCog.m_Position = uiAddCog.m_Position;
            addCog.m_Rotation = uiAddCog.m_Rotation;

            const flecs::entity undo = world.entity();
            auto& deleteCog = undo.ensure<xg::command::DeleteCogComponent>();
            deleteCog.m_CogId = uiAddCog.m_CogId;
            deleteCog.m_Position = uiAddCog.m_Position;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });

    world.each([&](flecs::entity entity, const xg::UIDeleteCogComponent& uiDeleteCog)
        {
            auto& deleteCog = entity.ensure<xg::command::DeleteCogComponent>();
            deleteCog.m_CogId = uiDeleteCog.m_CogId;
            deleteCog.m_Position = uiDeleteCog.m_Position;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::AddCogComponent>().m_CogId = uiDeleteCog.m_CogId;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });
    world.defer_end();
}