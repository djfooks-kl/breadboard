#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "Command/CommandRemovedFromHistoryComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandListComponent.h"
#include "Command/CommandUndoComponent.h"
#include "Command/CommandListSystem.h"
#include "Command/CommandToQueueComponent.h"
#include "UIUndoComponent.h"
#include "UIRedoComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::command::list_system - " description, "[xg::command::list_system]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::command::ListComponent>().m_Commands.resize(3);
            m_World.component<xg::command::RemovedFromHistoryComponent>();
            m_World.component<xg::command::ExecuteComponent>();
            m_World.component<xg::UIUndoComponent>();
            m_World.component<xg::UIRedoComponent>();
        }

        void Update()
        {
            xg::command::ListSystem::Update(m_World);

            m_World.defer_begin();
            m_World.each([](flecs::entity entity, xg::command::ToQueueComponent)
                {
                    entity.remove<xg::command::ToQueueComponent>();
                });

            m_World.each([](flecs::entity entity, xg::UIUndoComponent)
                {
                    entity.remove<xg::UIUndoComponent>();
                });

            m_World.each([](flecs::entity entity, xg::UIRedoComponent)
                {
                    entity.remove<xg::UIRedoComponent>();
                });
            m_World.defer_end();
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Add a command -> Put the command in the list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    flecs::entity undo = world.entity();
    command.ensure<xg::command::ToQueueComponent>().m_Undo = undo;

    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 1);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 1);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command);

    REQUIRE(command.has<xg::command::UndoComponent>());
    CHECK(command.get<xg::command::UndoComponent>().m_Undo == undo);
    CHECK(command.has<xg::command::ExecuteComponent>());
}

SYSTEM_TEST_CASE("Add a 2nd command -> Put both commands in the list")
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

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 1);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 1);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 2);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 2);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[1] == command2);

    REQUIRE(command1.has<xg::command::UndoComponent>());
    CHECK(command1.get<xg::command::UndoComponent>().m_Undo == undo1);
    CHECK(command1.has<xg::command::ExecuteComponent>() == false);

    REQUIRE(command2.has<xg::command::UndoComponent>());
    CHECK(command2.get<xg::command::UndoComponent>().m_Undo == undo2);
    CHECK(command2.has<xg::command::ExecuteComponent>());
}

SYSTEM_TEST_CASE("Add a 4 commands and overflow -> Wrap around and mark command1 for destruct then next frame destruct it")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity command3 = world.entity();
    flecs::entity undo3 = world.entity();
    flecs::entity command4 = world.entity();
    flecs::entity undo4 = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();
    command3.ensure<xg::command::ToQueueComponent>().m_Undo = undo3;
    env.Update();
    command4.ensure<xg::command::ToQueueComponent>().m_Undo = undo4;
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 3);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 3);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command4);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[1] == command2);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[2] == command3);

    CHECK(command1.has<xg::command::RemovedFromHistoryComponent>());
    CHECK(command2.has<xg::command::RemovedFromHistoryComponent>() == false);
    CHECK(command3.has<xg::command::RemovedFromHistoryComponent>() == false);
    CHECK(command4.has<xg::command::RemovedFromHistoryComponent>() == false);

    env.Update();
    CHECK(command1.is_alive() == false);
    CHECK(undo1.is_alive() == false);
}

SYSTEM_TEST_CASE("Add 1 command then undo it -> Run the Undo and remove it from the undo list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity ui = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 1);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);

    REQUIRE(command1.has<xg::command::UndoComponent>());
    CHECK(command1.get<xg::command::UndoComponent>().m_Undo == undo1);

    CHECK(command1.has<xg::command::ExecuteComponent>() == false);
    CHECK(undo1.has<xg::command::ExecuteComponent>());
}

SYSTEM_TEST_CASE("Add 2 commands then undo each -> Run the Undo and remove it from the undo list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity ui = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();

    {
        INFO("1st Undo");
        ui.add<xg::UIUndoComponent>();
        env.Update();

        CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 1);
        CHECK(world.get<xg::command::ListComponent>().m_Count == 2);
        CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
        CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 1);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[1] == command2);

        CHECK(command1.has<xg::command::ExecuteComponent>() == false);
        CHECK(command2.has<xg::command::ExecuteComponent>() == false);
        CHECK(undo1.has<xg::command::ExecuteComponent>() == false);
        CHECK(undo2.has<xg::command::ExecuteComponent>());
    }

    {
        INFO("2nd Undo");
        ui.add<xg::UIUndoComponent>();
        env.Update();

        CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 1);
        CHECK(world.get<xg::command::ListComponent>().m_Count == 2);
        CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
        CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 0);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[1] == command2);

        CHECK(command1.has<xg::command::ExecuteComponent>() == false);
        CHECK(command2.has<xg::command::ExecuteComponent>() == false);
        CHECK(undo1.has<xg::command::ExecuteComponent>());
        CHECK(undo2.has<xg::command::ExecuteComponent>() == false);
    }
}

SYSTEM_TEST_CASE("No commands and undo added -> Ignore it")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity ui = world.entity();
    ui.add<xg::UIUndoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == flecs::entity());
}

SYSTEM_TEST_CASE("1 command and undo added 2 times -> Ignore 2nd add")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command = world.entity();
    flecs::entity undo = world.entity();
    flecs::entity ui = world.entity();
    command.ensure<xg::command::ToQueueComponent>().m_Undo = undo;
    env.Update();

    ui.add<xg::UIUndoComponent>();
    env.Update();

    ui.add<xg::UIUndoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 1);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command);

    CHECK(command.has<xg::command::ExecuteComponent>() == false);
    CHECK(undo.has<xg::command::ExecuteComponent>() == false);
}

SYSTEM_TEST_CASE("Add 4 commands and overflow then undo -> Undo head index wraps around")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity command3 = world.entity();
    flecs::entity undo3 = world.entity();
    flecs::entity command4 = world.entity();
    flecs::entity undo4 = world.entity();
    flecs::entity ui = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();
    command3.ensure<xg::command::ToQueueComponent>().m_Undo = undo3;
    env.Update();
    command4.ensure<xg::command::ToQueueComponent>().m_Undo = undo4;
    env.Update();

    ui.add<xg::UIUndoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 2);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 3);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 2);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command4);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[1] == command2);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[2] == command3);

    CHECK(command1.is_alive() == false);
    CHECK(command2.has<xg::command::ExecuteComponent>() == false);
    CHECK(command3.has<xg::command::ExecuteComponent>() == false);
    CHECK(command4.has<xg::command::ExecuteComponent>() == false);

    CHECK(undo1.is_alive() == false);
    CHECK(undo2.has<xg::command::ExecuteComponent>() == false);
    CHECK(undo3.has<xg::command::ExecuteComponent>() == false);
    CHECK(undo4.has<xg::command::ExecuteComponent>());
}

SYSTEM_TEST_CASE("Add 1 command, undo it and then redo it -> Execute the Redo and add it back to the undo list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity ui = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();
    ui.add<xg::UIRedoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 1);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 1);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);

    CHECK(command1.has<xg::command::ExecuteComponent>() == true);
    CHECK(undo1.has<xg::command::ExecuteComponent>() == false);
}

SYSTEM_TEST_CASE("Add 2 commands, undo them and then redo each -> Execute each Redo and add the commands back to the undo list")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity ui = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();
    {
        INFO("1st redo");
        ui.add<xg::UIRedoComponent>();
        env.Update();

        CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 1);
        CHECK(world.get<xg::command::ListComponent>().m_Count == 2);
        CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
        CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 1);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[1] == command2);

        CHECK(command1.has<xg::command::ExecuteComponent>());
        CHECK(command2.has<xg::command::ExecuteComponent>() == false);
        CHECK(undo1.has<xg::command::ExecuteComponent>() == false);
        CHECK(undo2.has<xg::command::ExecuteComponent>() == false);
    }
    {
        INFO("2nd redo");
        ui.add<xg::UIRedoComponent>();
        env.Update();

        CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 1);
        CHECK(world.get<xg::command::ListComponent>().m_Count == 2);
        CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 1);
        CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 2);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);
        CHECK(world.get<xg::command::ListComponent>().m_Commands[1] == command2);

        CHECK(command1.has<xg::command::ExecuteComponent>() == false);
        CHECK(command2.has<xg::command::ExecuteComponent>());
        CHECK(undo1.has<xg::command::ExecuteComponent>() == false);
        CHECK(undo2.has<xg::command::ExecuteComponent>() == false);
    }
}

SYSTEM_TEST_CASE("No commands and redo added -> Ignore it")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity ui = world.entity();
    ui.add<xg::UIRedoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == flecs::entity());
}

SYSTEM_TEST_CASE("1 command, undo and then redo added twice -> Ignore 2nd redo")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity ui = world.entity();
    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();
    ui.add<xg::UIRedoComponent>();
    env.Update();
    ui.add<xg::UIRedoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 1);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 1);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command1);

    CHECK(command1.has<xg::command::ExecuteComponent>() == false);
    CHECK(undo1.has<xg::command::ExecuteComponent>() == false);
}

SYSTEM_TEST_CASE("4 commands causes wrap around, undo and redo them all -> redo wraps around correctly")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity ui = world.entity();

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity command3 = world.entity();
    flecs::entity undo3 = world.entity();
    flecs::entity command4 = world.entity();
    flecs::entity undo4 = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();
    command3.ensure<xg::command::ToQueueComponent>().m_Undo = undo3;
    env.Update();
    command4.ensure<xg::command::ToQueueComponent>().m_Undo = undo4;
    env.Update();

    ui.add<xg::UIUndoComponent>();
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();

    ui.add<xg::UIRedoComponent>();
    env.Update();
    ui.add<xg::UIRedoComponent>();
    env.Update();
    ui.add<xg::UIRedoComponent>();
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 3);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 3);

    CHECK(command1.is_alive() == false);
    CHECK(command2.has<xg::command::ExecuteComponent>() == false);
    CHECK(command3.has<xg::command::ExecuteComponent>() == false);
    CHECK(command4.has<xg::command::ExecuteComponent>());

    CHECK(undo1.is_alive() == false);
    CHECK(undo2.has<xg::command::ExecuteComponent>() == false);
    CHECK(undo3.has<xg::command::ExecuteComponent>() == false);
    CHECK(undo4.has<xg::command::ExecuteComponent>() == false);
}

SYSTEM_TEST_CASE("Add a command then undo once and queue a new command -> New command replaces the old command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity command1 = world.entity();
    flecs::entity undo1 = world.entity();
    flecs::entity command2 = world.entity();
    flecs::entity undo2 = world.entity();
    flecs::entity ui = world.entity();

    command1.ensure<xg::command::ToQueueComponent>().m_Undo = undo1;
    env.Update();
    ui.add<xg::UIUndoComponent>();
    env.Update();
    command2.ensure<xg::command::ToQueueComponent>().m_Undo = undo2;
    env.Update();

    CHECK(world.get<xg::command::ListComponent>().m_HeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_Count == 1);
    CHECK(world.get<xg::command::ListComponent>().m_UndoHeadIndex == 0);
    CHECK(world.get<xg::command::ListComponent>().m_UndoCount == 1);
    CHECK(world.get<xg::command::ListComponent>().m_Commands[0] == command2);

    CHECK(command1.has<xg::command::RemovedFromHistoryComponent>());
    CHECK(command2.has<xg::command::RemovedFromHistoryComponent>() == false);

    CHECK(command1.has<xg::command::ExecuteComponent>() == false);
    CHECK(command2.has<xg::command::ExecuteComponent>());

    CHECK(undo1.is_alive() == false);

    env.Update();
    CHECK(command1.is_alive() == false);
}
