#include "BreadTest.h"
#include <flecs/flecs.h>

#include "Command/CommandAddWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandMoveGroupComponent.h"
#include "WireComponent.h"
#include "WireSystem.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::WireSystem - " description, "[xg::WireSystem]")

namespace
{
    struct TestEnv
    {
        void Update()
        {
            xg::WireSystem::Update(m_World);

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

SYSTEM_TEST_CASE("Executing an add command -> Copy the added values onto the WireComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    {
        auto& addWire = commandAdd.ensure<xg::command::AddWireComponent>();
        addWire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        addWire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();

    env.Update();
    REQUIRE(addedEntity.has<xg::WireComponent>());
    REQUIRE(addedEntity.get<xg::WireComponent>().m_Checkpoints.size() == 2);
    CHECK(addedEntity.get<xg::WireComponent>().m_Checkpoints[0] == glm::ivec2(0, 0));
    CHECK(addedEntity.get<xg::WireComponent>().m_Checkpoints[1] == glm::ivec2(1, 1));
}

SYSTEM_TEST_CASE("Executing an add command, and then another with no checkpoints -> Ignore the second command")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    {
        auto& addWire = commandAdd.ensure<xg::command::AddWireComponent>();
        addWire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        addWire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();

    env.Update();

    flecs::entity commandRedoAdd = world.entity();
    commandRedoAdd.ensure<xg::command::AddWireComponent>();
    commandRedoAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandRedoAdd.add<xg::command::ExecuteComponent>();

    env.Update();
    REQUIRE(addedEntity.has<xg::WireComponent>());
    REQUIRE(addedEntity.get<xg::WireComponent>().m_Checkpoints.size() == 2);
    CHECK(addedEntity.get<xg::WireComponent>().m_Checkpoints[0] == glm::ivec2(0, 0));
    CHECK(addedEntity.get<xg::WireComponent>().m_Checkpoints[1] == glm::ivec2(1, 1));
}

SYSTEM_TEST_CASE("Moving wires -> Update the segments")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity otherMovedEntity = world.entity();

    flecs::entity addedEntity1 = world.entity();
    flecs::entity commandAdd1 = world.entity();
    {
        auto& addWire = commandAdd1.ensure<xg::command::AddWireComponent>();
        addWire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        addWire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }
    commandAdd1.ensure<xg::command::EntityComponent>().m_Entity = addedEntity1;
    commandAdd1.add<xg::command::ExecuteComponent>();

    flecs::entity addedEntity2 = world.entity();
    flecs::entity commandAdd2 = world.entity();
    {
        auto& addWire = commandAdd2.ensure<xg::command::AddWireComponent>();
        addWire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        addWire.m_Checkpoints.push_back(glm::ivec2(2, 2));
    }
    commandAdd2.ensure<xg::command::EntityComponent>().m_Entity = addedEntity2;
    commandAdd2.add<xg::command::ExecuteComponent>();

    env.Update();

    flecs::entity commandMove = world.entity();
    {
        auto& moveGroup = commandMove.ensure<xg::command::MoveGroupComponent>();
        moveGroup.m_Entities.push_back(otherMovedEntity);
        moveGroup.m_Entities.push_back(addedEntity1);
        moveGroup.m_Entities.push_back(addedEntity2);
        moveGroup.m_Transform = xc::ITransform{ .m_Translation = glm::ivec2(10, 10), .m_Rotation = xc::Rotation90(2) };
    }
    commandMove.add<xg::command::ExecuteComponent>();

    env.Update();

    REQUIRE(addedEntity1.has<xg::WireComponent>());
    REQUIRE(addedEntity1.get<xg::WireComponent>().m_Checkpoints.size() == 2);
    CHECK(addedEntity1.get<xg::WireComponent>().m_Checkpoints[0] == glm::ivec2(10, 10));
    CHECK(addedEntity1.get<xg::WireComponent>().m_Checkpoints[1] == glm::ivec2(9, 9));

    REQUIRE(addedEntity2.has<xg::WireComponent>());
    REQUIRE(addedEntity2.get<xg::WireComponent>().m_Checkpoints.size() == 2);
    CHECK(addedEntity2.get<xg::WireComponent>().m_Checkpoints[0] == glm::ivec2(10, 10));
    CHECK(addedEntity2.get<xg::WireComponent>().m_Checkpoints[1] == glm::ivec2(8, 8));
}
