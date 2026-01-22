#include <catch2/catch_test_macros.hpp>
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
        TestEnv()
        {
            m_World.emplace<xg::OnStageAddedComponent>();
            m_World.emplace<xg::OnStageRemovedComponent>();
        }

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

SYSTEM_TEST_CASE("Executing an add command -> Add the OnStageComponent and add the entity to OnStageAddedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddCogComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;

    env.Update();
    CHECK(addedEntity.has<xg::OnStageComponent>() == false);

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::OnStageComponent>());
    REQUIRE(world.get<xg::OnStageAddedComponent>().m_Entities.size() == 1);
    CHECK(world.get<xg::OnStageAddedComponent>().m_Entities[0] == addedEntity);
}

SYSTEM_TEST_CASE("Executing a delete command -> Remove the OnStageComponent and add the entity to OnStageRemovedComponent")
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
    REQUIRE(world.get<xg::OnStageRemovedComponent>().m_Entities.size() == 1);
    CHECK(world.get<xg::OnStageRemovedComponent>().m_Entities[0] == addedEntity);
}

SYSTEM_TEST_CASE("No entities added/removed on the frame -> Clear the OnStageAddedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    commandAdd.ensure<xg::command::AddCogComponent>();
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    env.Update();
    CHECK(world.get<xg::OnStageAddedComponent>().m_Entities.empty());

    flecs::entity commandDelete = world.entity();
    commandDelete.ensure<xg::command::DeleteCogComponent>().m_Cog = addedEntity;
    commandDelete.add<xg::command::ExecuteComponent>();
    env.Update();

    env.Update();
    CHECK(world.get<xg::OnStageRemovedComponent>().m_Entities.empty());
}
