#include "BreadTest.h"
#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandAddWireComponent.h"
#include "Command/CommandCreateSystem.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandDeleteWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandRemovedFromHistoryComponent.h"
#include "Command/CommandToQueueComponent.h"
#include "FlecsTestHelpers.h"
#include "UIAddCogComponent.h"
#include "UIDeleteCogComponent.h"
#include "UIAddWireComponent.h"
#include "UIDeleteWireComponent.h"

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
                    entity.destruct();
                });

            m_World.each([](flecs::entity entity, xg::UIDeleteCogComponent)
                {
                    entity.destruct();
                });

            m_World.each([](flecs::entity entity, xg::UIAddWireComponent)
                {
                    entity.destruct();
                });

            m_World.each([](flecs::entity entity, xg::UIDeleteWireComponent)
                {
                    entity.destruct();
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

    const flecs::entity uiCommand = world.entity();
    auto& uiAddCog = uiCommand.ensure<xg::UIAddCogComponent>();
    uiAddCog.m_CogId = s_TestCog1;
    uiAddCog.m_Transform = { glm::ivec2(2, 3), xc::Rotation90(1) };
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::AddCogComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];
    CHECK(uiCommand != command);

    REQUIRE(command.has<xg::command::AddCogComponent>());
    CHECK(command.get<const xg::command::AddCogComponent>().m_CogId == s_TestCog1);
    CHECK(command.get<const xg::command::AddCogComponent>().m_Transform == xc::ITransform{ glm::ivec2(2, 3), xc::Rotation90(1) });

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

    flecs::entity uiCommand = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& deleteCog = uiCommand.ensure<xg::UIDeleteCogComponent>();
    deleteCog.m_Cog = createdEntity;
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::DeleteCogComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];
    CHECK(uiCommand != command);

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
    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::AddCogComponent>());
    env.Update();

    CHECK(entities[0].has<xg::command::ToQueueComponent>() == false);
}

TEST_CASE("When a delete cog command is destructed destroy the added entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiCommand = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& deleteCog = uiCommand.ensure<xg::UIDeleteCogComponent>();
    deleteCog.m_Cog = createdEntity;
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::DeleteCogComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    command.add<xg::command::RemovedFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(createdEntity) == false);
    CHECK(world.is_alive(command) == false);
}

TEST_CASE("Add a wire -> Queue for execution and add a delete wire undo command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    const flecs::entity uiCommand = world.entity();
    auto& uiAddWire = uiCommand.ensure<xg::UIAddWireComponent>();
    uiAddWire.m_Checkpoints.push_back(glm::ivec2(0, 0));
    uiAddWire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::AddWireComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];
    CHECK(uiCommand != command);

    REQUIRE(command.has<xg::command::AddWireComponent>());
    REQUIRE(command.get<const xg::command::AddWireComponent>().m_Checkpoints.size() == 2);
    CHECK(command.get<const xg::command::AddWireComponent>().m_Checkpoints[0] == glm::ivec2(0, 0));
    CHECK(command.get<const xg::command::AddWireComponent>().m_Checkpoints[1] == glm::ivec2(1, 1));

    REQUIRE(command.has<xg::command::EntityComponent>());
    CHECK(command.get<const xg::command::EntityComponent>().m_Entity != flecs::entity::null());
    const flecs::entity createdEntity = command.get<const xg::command::EntityComponent>().m_Entity;

    REQUIRE(command.has<xg::command::ToQueueComponent>());
    const flecs::entity undo = command.get<const xg::command::ToQueueComponent>().m_Undo;
    REQUIRE(undo != flecs::entity());
    REQUIRE(undo.has<xg::command::DeleteWireComponent>());
    CHECK(undo.get<const xg::command::DeleteWireComponent>().m_Wire == createdEntity);
}

TEST_CASE("Delete a wire -> Queue for execution and add an add wire undo command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiCommand = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& deleteWire = uiCommand.ensure<xg::UIDeleteWireComponent>();
    deleteWire.m_Wire = createdEntity;
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::DeleteWireComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];
    CHECK(uiCommand != command);

    REQUIRE(command.has<xg::command::DeleteWireComponent>());
    CHECK(command.get<const xg::command::DeleteWireComponent>().m_Wire == createdEntity);

    REQUIRE(command.has<xg::command::ToQueueComponent>());
    flecs::entity undo = command.get<const xg::command::ToQueueComponent>().m_Undo;
    REQUIRE(undo != flecs::entity());
    REQUIRE(undo.has<xg::command::AddWireComponent>());
    CHECK(undo.get<const xg::command::AddWireComponent>().m_Checkpoints.empty());

    REQUIRE(undo.has<xg::command::EntityComponent>());
    CHECK(undo.get<const xg::command::EntityComponent>().m_Entity == createdEntity);
}

TEST_CASE("When a delete wire command is destructed destroy the added entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiCommand = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& deleteWire = uiCommand.ensure<xg::UIDeleteWireComponent>();
    deleteWire.m_Wire = createdEntity;
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::DeleteWireComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    command.add<xg::command::RemovedFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(createdEntity) == false);
    CHECK(world.is_alive(command) == false);
}