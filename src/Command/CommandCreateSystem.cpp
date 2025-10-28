#include "Command/CommandCreateSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandToQueueComponent.h"
#include "UIAddCogComponent.h"
#include "UIDeleteCogComponent.h"

void xg::command::create_system::Update(flecs::world& world)
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
            entity.ensure<xg::command::AddCogComponent>().m_CogId = uiAddCog.m_CogId;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::DeleteCogComponent>().m_Position = glm::ivec2(1, 1);

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });

    world.each([&](flecs::entity entity, const xg::UIDeleteCogComponent& uiDeleteCog)
        {
            entity.ensure<xg::command::DeleteCogComponent>().m_Position = glm::ivec2(1, 1);

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::AddCogComponent>().m_CogId = uiDeleteCog.m_CogId;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });
    world.defer_end();
}