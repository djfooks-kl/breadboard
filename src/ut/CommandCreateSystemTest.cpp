#include "BreadTest.h"
#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandAddGroupComponent.h"
#include "Command/CommandAddWireComponent.h"
#include "Command/CommandCreateSystem.h"
#include "Command/CommandDeleteCogComponent.h"
#include "Command/CommandDeleteGroupComponent.h"
#include "Command/CommandDeleteWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExpiredFromHistoryComponent.h"
#include "Command/CommandMoveGroupComponent.h"
#include "Command/CommandSeveredFromHistoryComponent.h"
#include "Command/CommandToQueueComponent.h"
#include "FlecsTestHelpers.h"
#include "UIAddCogComponent.h"
#include "UIAddWireComponent.h"
#include "UIDeleteCogComponent.h"
#include "UIDeleteWireComponent.h"
#include "UIDragDeleteGroupComponent.h"
#include "UIMoveGroupComponent.h"

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

            m_World.each([](flecs::entity entity, xg::UIMoveGroupComponent)
                {
                    entity.destruct();
                });

            m_World.each([](flecs::entity entity, xg::UIDragDeleteGroupComponent)
                {
                    entity.destruct();
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

    env.Update();
    CHECK(createdEntity.is_alive());
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

TEST_CASE("When a delete cog command is expires from history destroy the added entity")
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

    command.add<xg::command::ExpiredFromHistoryComponent>();
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

TEST_CASE("Delete a wire -> Queue for execution and add an add wire undo command (do not destroy the entity yet)")
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

    env.Update();
    CHECK(createdEntity.is_alive());
}

TEST_CASE("When a delete wire command expires from history destroy the added entity")
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

    command.add<xg::command::ExpiredFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(createdEntity) == false);
    CHECK(world.is_alive(command) == false);
}

TEST_CASE("When an add wire command is severed from history destroy the added entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiCommand = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& uiAddWire = uiCommand.ensure<xg::UIAddWireComponent>();
    uiAddWire.m_Checkpoints.push_back(glm::ivec2(0, 0));
    uiAddWire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::AddWireComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    command.add<xg::command::SeveredFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(command) == false);
}

TEST_CASE("When an add cog command is severed from history destroy the added entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiCommand = world.entity();
    flecs::entity createdEntity = world.entity();
    auto& uiAddCog = uiCommand.ensure<xg::UIAddCogComponent>();
    uiAddCog.m_CogId = s_TestCog1;
    uiAddCog.m_Transform = { glm::ivec2(2, 3), xc::Rotation90(1) };
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::AddCogComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    command.add<xg::command::SeveredFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(command) == false);
}

TEST_CASE("Move group -> Queue for execution and add a move back undo command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    const flecs::entity uiCommand = world.entity();
    flecs::entity entity1 = world.entity();
    flecs::entity entity2 = world.entity();
    auto& uiMove = uiCommand.ensure<xg::UIMoveGroupComponent>();
    uiMove.m_Entities.push_back(entity1);
    uiMove.m_Entities.push_back(entity2);
    xc::ITransform transform{ .m_Translation = glm::ivec2(5, 11), .m_Rotation = xc::Rotation90(3) };
    uiMove.m_Transform = transform;
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::MoveGroupComponent, xg::command::ToQueueComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];
    CHECK(uiCommand != command);

    REQUIRE(command.get<const xg::command::MoveGroupComponent>().m_Entities.size() == 2);
    CHECK(command.get<const xg::command::MoveGroupComponent>().m_Entities[0] == entity1);
    CHECK(command.get<const xg::command::MoveGroupComponent>().m_Entities[1] == entity2);
    CHECK(command.get<const xg::command::MoveGroupComponent>().m_Transform == transform);

    REQUIRE(command.has<xg::command::ToQueueComponent>());
    const flecs::entity undo = command.get<const xg::command::ToQueueComponent>().m_Undo;
    REQUIRE(undo != flecs::entity());
    REQUIRE(undo.has<xg::command::MoveGroupComponent>());
    REQUIRE(undo.get<const xg::command::MoveGroupComponent>().m_Entities.size() == 2);
    CHECK(undo.get<const xg::command::MoveGroupComponent>().m_Entities[0] == entity1);
    CHECK(undo.get<const xg::command::MoveGroupComponent>().m_Entities[1] == entity2);
    CHECK(undo.get<const xg::command::MoveGroupComponent>().m_Transform == transform.Inverse());
}

TEST_CASE("When a move group command expires from history destroy the command entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    const flecs::entity uiCommand = world.entity();
    flecs::entity entity1 = world.entity();
    flecs::entity entity2 = world.entity();
    auto& uiMove = uiCommand.ensure<xg::UIMoveGroupComponent>();
    uiMove.m_Entities.push_back(entity1);
    uiMove.m_Entities.push_back(entity2);
    xc::ITransform transform{ .m_Translation = glm::ivec2(5, 11), .m_Rotation = xc::Rotation90(3) };
    uiMove.m_Transform = transform;
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::ToQueueComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    command.add<xg::command::ExpiredFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(command) == false);
}

TEST_CASE("When a move group command is severed from history destroy the command entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    const flecs::entity uiCommand = world.entity();
    flecs::entity entity1 = world.entity();
    flecs::entity entity2 = world.entity();
    auto& uiMove = uiCommand.ensure<xg::UIMoveGroupComponent>();
    uiMove.m_Entities.push_back(entity1);
    uiMove.m_Entities.push_back(entity2);
    xc::ITransform transform{ .m_Translation = glm::ivec2(5, 11), .m_Rotation = xc::Rotation90(3) };
    uiMove.m_Transform = transform;
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::ToQueueComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    command.add<xg::command::SeveredFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(command) == false);
}

TEST_CASE("Delete a group (drag) -> Queue for execution and add an add group undo command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiCommand = world.entity();
    flecs::entity entity1 = world.entity();
    flecs::entity entity2 = world.entity();
    auto& deleteGroup = uiCommand.ensure<xg::UIDragDeleteGroupComponent>();
    deleteGroup.m_Entities.push_back(entity1);
    deleteGroup.m_Entities.push_back(entity2);
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::ToQueueComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    REQUIRE(command.get<const xg::command::DeleteGroupComponent>().m_Entities.size() == 2);
    CHECK(command.get<const xg::command::DeleteGroupComponent>().m_Entities[0] == entity1);
    CHECK(command.get<const xg::command::DeleteGroupComponent>().m_Entities[1] == entity2);

    REQUIRE(command.has<xg::command::ToQueueComponent>());
    const flecs::entity undo = command.get<const xg::command::ToQueueComponent>().m_Undo;
    REQUIRE(undo != flecs::entity());
    REQUIRE(undo.has<xg::command::AddGroupComponent>());
    REQUIRE(undo.get<const xg::command::AddGroupComponent>().m_Entities.size() == 2);
    CHECK(undo.get<const xg::command::AddGroupComponent>().m_Entities[0] == entity1);
    CHECK(undo.get<const xg::command::AddGroupComponent>().m_Entities[1] == entity2);
}

TEST_CASE("When a delete group command expires from history destroy the added entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiCommand = world.entity();
    flecs::entity entity1 = world.entity();
    flecs::entity entity2 = world.entity();
    auto& deleteGroup = uiCommand.ensure<xg::UIDragDeleteGroupComponent>();
    deleteGroup.m_Entities.push_back(entity1);
    deleteGroup.m_Entities.push_back(entity2);
    env.Update();

    std::vector<flecs::entity> entities = xc::ut::CollectEntities(world.query<xg::command::DeleteGroupComponent>());
    REQUIRE(entities.size() == 1);
    flecs::entity command = entities[0];

    command.add<xg::command::ExpiredFromHistoryComponent>();
    env.Update();

    CHECK(world.is_alive(entity1) == false);
    CHECK(world.is_alive(entity2) == false);
    CHECK(world.is_alive(command) == false);
}
