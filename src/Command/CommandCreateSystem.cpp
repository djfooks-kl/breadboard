#include "Command/CommandCreateSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandAddWireComponent.h"
#include "Command/CommandDeleteWireComponent.h"
#include "Command/CommandRemovedFromHistoryComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandToQueueComponent.h"
#include "UIAddCogComponent.h"
#include "UIDeleteCogComponent.h"
#include "UIPreviewCreateWireComponent.h"
#include "UIAddWireComponent.h"
#include "UIDeleteWireComponent.h"

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
            deleteCog.m_Cog.destruct();
            entity.destruct();
        });
    world.defer_end();

    world.defer_begin();
    world.each([](
        flecs::entity entity,
        xg::command::RemovedFromHistoryComponent,
        xg::command::DeleteWireComponent& deleteWire)
        {
            deleteWire.m_Wire.destruct();
            entity.destruct();
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](const xg::UIAddCogComponent& uiAddCog)
        {
            flecs::entity entity = world.entity();
            auto& addCog = entity.ensure<xg::command::AddCogComponent>();
            addCog.m_CogId = uiAddCog.m_CogId;
            addCog.m_Transform = uiAddCog.m_Transform;

            flecs::entity addEntity = world.entity();
            entity.ensure<xg::command::EntityComponent>().m_Entity = addEntity;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::DeleteCogComponent>().m_Cog = addEntity;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });

    world.each([&](const xg::UIDeleteCogComponent& uiDeleteCog)
        {
            flecs::entity entity = world.entity();
            auto& deleteCog = entity.ensure<xg::command::DeleteCogComponent>();
            deleteCog.m_Cog = uiDeleteCog.m_Cog;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::AddCogComponent>();

            undo.ensure<xg::command::EntityComponent>().m_Entity = deleteCog.m_Cog;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });

    world.each([&](const xg::UIAddWireComponent& uiAddWire)
        {
            flecs::entity entity = world.entity();
            auto& addWire = entity.ensure<xg::command::AddWireComponent>();
            addWire.m_Checkpoints = uiAddWire.m_Checkpoints;

            flecs::entity addEntity = world.entity();
            entity.ensure<xg::command::EntityComponent>().m_Entity = addEntity;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::DeleteWireComponent>().m_Wire = addEntity;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });

    world.each([&](const xg::UIDeleteWireComponent& uiDeleteWire)
        {
            flecs::entity entity = world.entity();
            auto& deleteWire = entity.ensure<xg::command::DeleteWireComponent>();
            deleteWire.m_Wire = uiDeleteWire.m_Wire;

            const flecs::entity undo = world.entity();
            undo.ensure<xg::command::AddWireComponent>();

            undo.ensure<xg::command::EntityComponent>().m_Entity = deleteWire.m_Wire;

            entity.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
        });

    world.defer_end();
}