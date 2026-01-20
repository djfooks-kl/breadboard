#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandEntitySystemTest.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::command::EntitySystem - " description, "[xg::command::EntitySystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.component<xg::command::ExecuteComponent>();
            m_World.component<xg::command::EntityComponent>();
        }

        void Update()
        {
            xg::command::EntitySystem::Update(m_World);

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

SYSTEM_TEST_CASE("Add a cog first time -> Create a new entity with OnStageComponent and add a EntityComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    command.add<xg::command::ExecuteComponent>();
    command.add<xg::command::AddCogComponent>();

    env.Update();

    REQUIRE(command.has<xg::command::EntityComponent>());
    CHECK(command.get<xg::command::EntityComponent>().m_Entity != command);
    CHECK(command.get<xg::command::EntityComponent>().m_Entity != flecs::entity::null());
}

SYSTEM_TEST_CASE("Add the cog a 2nd time -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    command.add<xg::command::ExecuteComponent>();
    command.add<xg::command::AddCogComponent>();
    env.Update();
    REQUIRE(command.has<xg::command::EntityComponent>());
    flecs::entity newEntity = command.get<xg::command::EntityComponent>().m_Entity;

    command.add<xg::command::ExecuteComponent>();
    command.add<xg::command::AddCogComponent>();
    env.Update();

    REQUIRE(command.has<xg::command::EntityComponent>());
    CHECK(command.get<xg::command::EntityComponent>().m_Entity == newEntity);
}

SYSTEM_TEST_CASE("Add a cog and then remove it -> Remove the OnStageComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    command1.add<xg::command::ExecuteComponent>();
    command1.add<xg::command::AddCogComponent>();
    env.Update();
    REQUIRE(command1.has<xg::command::EntityComponent>());
    flecs::entity newEntity = command1.get<xg::command::EntityComponent>().m_Entity;

    flecs::entity command2 = world.entity();
    command2.add<xg::command::ExecuteComponent>();
    command2.add<xg::command::DeleteCogComponent>().m_Position = wtf?
    env.Update();

    REQUIRE(command.has<xg::command::EntityComponent>());
    CHECK(command.get<xg::command::EntityComponent>().m_Entity == newEntity);
}

SYSTEM_TEST_CASE("After adding a cog the command is destructed -> Do nothing")
{
}
