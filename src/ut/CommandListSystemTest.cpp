#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "Command/CommandExecuteComponent.h"
#include "Command/CommandListComponent.h"
#include "Command/CommandListItemComponent.h"
#include "Command/CommandListSizeComponent.h"
#include "Command/CommandListSystem.h"
#include "Command/CommandToQueueComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::command::ListSystem - " description, "[xg::command::ListSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.emplace<xg::command::ListComponent>();
            m_World.emplace<xg::command::ListSizeComponent>();
            m_World.component<xg::command::ExecuteComponent>();
        }

        void Update()
        {
            xg::command::list_system::Update(m_World);

			m_World.defer_begin();
            auto query = m_World.query_builder<
                const xg::command::ToQueueComponent>();
            query.each([](flecs::entity entity, const xg::command::ToQueueComponent&)
					{
						entity.remove<xg::command::ToQueueComponent>();
					});
            m_World.defer_end();
        }

        flecs::world m_World;
    };
}

TEST_CASE("Add a command -> Put the command in the list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    flecs::entity undo = world.entity();
    command.ensure<xg::command::ToQueueComponent>().m_Undo = undo;

    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_Head == command);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHead == command);
    CHECK(world.get<xg::command::ListComponent>().m_Tail == command);

    REQUIRE(command.has<xg::command::ListItemComponent>());
    CHECK(command.get<xg::command::ListItemComponent>().m_Next == flecs::entity());
    CHECK(command.get<xg::command::ListItemComponent>().m_Undo == undo);
    CHECK(command.get<xg::command::ListItemComponent>().m_Previous == flecs::entity());
    CHECK(command.has<xg::command::ExecuteComponent>());
}

TEST_CASE("Add a 2nd command -> Put both commands in the list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_Head == command2);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHead == command2);
    CHECK(world.get<xg::command::ListComponent>().m_Tail == command1);

    REQUIRE(command1.has<xg::command::ListItemComponent>());
    CHECK(command1.get<xg::command::ListItemComponent>().m_Next == command2);
    CHECK(command1.get<xg::command::ListItemComponent>().m_Undo == undo1);
    CHECK(command1.get<xg::command::ListItemComponent>().m_Previous == flecs::entity());
    CHECK(command1.has<xg::command::ExecuteComponent>() == false);

    REQUIRE(command2.has<xg::command::ListItemComponent>());
    CHECK(command2.get<xg::command::ListItemComponent>().m_Next == flecs::entity());
    CHECK(command2.get<xg::command::ListItemComponent>().m_Undo == undo2);
    CHECK(command2.get<xg::command::ListItemComponent>().m_Previous == command1);
    CHECK(command2.has<xg::command::ExecuteComponent>());
}

TEST_CASE("Add a 3rd command -> Put both commands in the list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity command3 = world.entity();
    flecs::entity undo3 = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();
    command3.ensure<xg::command::ToQueueComponent>().m_Undo = undo3;
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_Head == command3);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHead == command3);
    CHECK(world.get<xg::command::ListComponent>().m_Tail == command1);

    REQUIRE(command1.has<xg::command::ListItemComponent>());
    CHECK(command1.get<xg::command::ListItemComponent>().m_Next == command2);
    CHECK(command1.get<xg::command::ListItemComponent>().m_Undo == undo1);
    CHECK(command1.get<xg::command::ListItemComponent>().m_Previous == flecs::entity());
    CHECK(command1.has<xg::command::ExecuteComponent>() == false);

    REQUIRE(command2.has<xg::command::ListItemComponent>());
    CHECK(command2.get<xg::command::ListItemComponent>().m_Next == command3);
    CHECK(command2.get<xg::command::ListItemComponent>().m_Undo == undo2);
    CHECK(command2.get<xg::command::ListItemComponent>().m_Previous == command1);
    CHECK(command2.has<xg::command::ExecuteComponent>() == false);

    REQUIRE(command3.has<xg::command::ListItemComponent>());
    CHECK(command3.get<xg::command::ListItemComponent>().m_Next == flecs::entity());
    CHECK(command3.get<xg::command::ListItemComponent>().m_Undo == undo3);
    CHECK(command3.get<xg::command::ListItemComponent>().m_Previous == command2);
    CHECK(command3.has<xg::command::ExecuteComponent>());
}

TEST_CASE("Set size=2 then add a 3rd command -> Put both commands in the list and remove the 1st")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::command::ListSizeComponent>().m_Size = 2;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity command3 = world.entity();
    flecs::entity undo3 = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();
    command3.ensure<xg::command::ToQueueComponent>().m_Undo = undo3;
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_Head == command3);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHead == command3);
    CHECK(world.get<xg::command::ListComponent>().m_Tail == command2);

    CHECK(command1.is_alive() == false);
    CHECK(undo1.is_alive() == false);

    REQUIRE(command2.has<xg::command::ListItemComponent>());
    CHECK(command2.get<xg::command::ListItemComponent>().m_Next == command3);
    CHECK(command2.get<xg::command::ListItemComponent>().m_Undo == undo2);
    CHECK(command2.get<xg::command::ListItemComponent>().m_Previous == flecs::entity());
    CHECK(command2.has<xg::command::ExecuteComponent>() == false);

    REQUIRE(command3.has<xg::command::ListItemComponent>());
    CHECK(command3.get<xg::command::ListItemComponent>().m_Next == flecs::entity());
    CHECK(command3.get<xg::command::ListItemComponent>().m_Undo == undo3);
    CHECK(command3.get<xg::command::ListItemComponent>().m_Previous == command2);
    CHECK(command3.has<xg::command::ExecuteComponent>());
}