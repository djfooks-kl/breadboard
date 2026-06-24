#include "BreadTest.h"
#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageComponent.h"
#include "OnStageRemovedComponent.h"
#include "OnStageSystem.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::OnStageSystem - " description, "[xg::OnStageSystem]")

namespace
{
    struct TestEnv
    {
        void Update()
        {
            xg::OnStageSystem::Update(m_World);

            m_World.defer_begin();
            m_World.each([](flecs::entity entity, xg::command::ExecuteComponent)
                {
                    entity.remove<xg::command::ExecuteComponent>();
                });

            m_World.defer_end();
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Executing an add cog command -> "
    "Add the OnStageComponent and OnStageAddedComponent then next frame remove the OnStageAddedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddCogComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    env.Update();
    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageAddedComponent>() == false);

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    CHECK(addedEntity.has<xg::OnStageAddedComponent>());
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    CHECK(addedEntity.has<xg::OnStageAddedComponent>() == false);
}

SYSTEM_TEST_CASE("Executing a delete command -> "
    "Remove the OnStageComponent and add OnStageRemovedComponent then next frame remove the OnStageRemovedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddCogComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    flecs::entity commandDelete = world.entity();
    commandDelete.ensure<xg::command::DeleteCogComponent>().m_Cog = addedEntity;

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    commandDelete.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageRemovedComponent>());

    env.Update();
    CHECK(addedEntity.has<xg::OnStageComponent>() == false);
    CHECK(addedEntity.has<xg::OnStageRemovedComponent>() == false);
}
