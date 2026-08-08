#include "BreadTest.h"
#include <flecs/flecs.h>

#include "WireComponent.h"
#include "WireSystem.h"
#include "Command/CommandAddWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"

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