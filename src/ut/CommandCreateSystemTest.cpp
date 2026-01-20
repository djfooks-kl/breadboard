#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandCreateSystem.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandRemovedFromHistoryComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandToQueueComponent.h"
#include "UIAddCogComponent.h"
#include "UIDeleteCogComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::command::CreateSystem - " description, "[xg::command::CreateSystem]")

namespace
{
    static const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");

    struct TestEnv
    {
        TestEnv()
        {
        }

        void Update()
        {
            xg::command::CreateSystem::Update(m_World);

            m_World.defer_begin();
            m_World.each([](flecs::entity entity, xg::UIAddCogComponent)
                {
                    entity.remove<xg::UIAddCogComponent>();
                });

            m_World.each([](flecs::entity entity, xg::UIDeleteCogComponent)
                {
                    entity.remove<xg::UIDeleteCogComponent>();
                });

            m_World.each([](flecs::entity entity, xg::command::RemovedFromHistoryComponent)
                {
                    entity.remove<xg::command::RemovedFromHistoryComponent>();
                });

            m_World.defer_end();
        }

        flecs::world m_World;
    };
}

TEST_CASE("Add a cog -> Queue for execution and add a delete cog undo command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    const flecs::entity command = world.entity();
    auto& uiAddCog = command.ensure<xg::UIAddCogComponent>();
    uiAddCog.m_CogId = s_TestCog1;
    uiAddCog.m_Rotation = xc::Rotation90(1);
    uiAddCog.m_Position = glm::ivec2(2, 3);
    env.Update();

    REQUIRE(command.has<xg::command::AddCogComponent>());
    CHECK(command.get<const xg::command::AddCogComponent>().m_CogId == s_TestCog1);
    CHECK(command.get<const xg::command::AddCogComponent>().m_Rotation == xc::Rotation90(1));
    CHECK(command.get<const xg::command::AddCogComponent>().m_Position == glm::ivec2(2, 3));

    REQUIRE(command.has<xg::command::EntityComponent>());
    CHECK(command.get<const xg::command::EntityComponent>().m_Entity != flecs::entity::null());
    const flecs::entity createdEntity = command.get<const xg::command::EntityComponent>().m_Entity;

    REQUIRE(command.has<xg::command::ToQueueComponent>());
    const flecs::entity undo = command.get<const xg::command::ToQueueComponent>().m_Undo;
    REQUIRE(undo != flecs::entity());
    REQUIRE(undo.has<xg::command::DeleteCogComponent>());
    CHECK(undo.get<const xg::command::DeleteCogComponent>().m_Cog == createdEntity);
}

TEST_CASE("Delete a cog -> Queue for execution and add an add cog undo command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& deleteCog = command.ensure<xg::UIDeleteCogComponent>();
    deleteCog.m_Cog = createdEntity;
    env.Update();

    REQUIRE(command.has<xg::command::DeleteCogComponent>());
    CHECK(command.get<const xg::command::DeleteCogComponent>().m_Cog == createdEntity);

    REQUIRE(command.has<xg::command::ToQueueComponent>());
    flecs::entity undo = command.get<const xg::command::ToQueueComponent>().m_Undo;
    REQUIRE(undo != flecs::entity());
    REQUIRE(undo.has<xg::command::AddCogComponent>());
    CHECK(undo.get<const xg::command::AddCogComponent>().m_CogId.IsEmpty());

    REQUIRE(undo.has<xg::command::EntityComponent>());
    CHECK(undo.get<const xg::command::EntityComponent>().m_Entity == createdEntity);
}

TEST_CASE("After a command is queued the ToQueueComponent is removed")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    command.ensure<xg::UIAddCogComponent>().m_CogId = s_TestCog1;
    env.Update();
    env.Update();

    CHECK(command.has<xg::command::ToQueueComponent>() == false);
}

TEST_CASE("When a delete cog command is destructed destroy the added entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& deleteCog = command.ensure<xg::UIDeleteCogComponent>();
    deleteCog.m_Cog = createdEntity;
    env.Update();

    command.add<xg::command::RemovedFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(createdEntity) == false);
}
