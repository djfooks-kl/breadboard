#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "CogCreatedComponent.h"
#include "CogSystem.h"
#include "Command/CommandAddCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::CogSystem - " description, "[xg::CogSystem]")

namespace
{
    static const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");

    struct TestEnv
    {
        void Update()
        {
            xg::CogSystem::Update(m_World);

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

SYSTEM_TEST_CASE("Executing an add command -> Copy the added values onto the CogComponent and add a CogCreatedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    {
        auto& addCog = commandAdd.ensure<xg::command::AddCogComponent>();
        addCog.m_CogId = s_TestCog1;
        addCog.m_Position = glm::ivec2(1, 2);
        addCog.m_Rotation = xc::Rotation90(1);
    }
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();

    env.Update();
    CHECK(addedEntity.has<xg::CogCreatedComponent>());
    REQUIRE(addedEntity.has<xg::CogComponent>());
    CHECK(addedEntity.get<xg::CogComponent>().m_CogId == s_TestCog1);
    CHECK(addedEntity.get<xg::CogComponent>().m_Position == glm::ivec2(1, 2));
    CHECK(addedEntity.get<xg::CogComponent>().m_Rotation == xc::Rotation90(1));

    env.Update();
    CHECK(addedEntity.has<xg::CogCreatedComponent>() == false);
}

SYSTEM_TEST_CASE("Executing an add command a 2nd time -> Do not add a CogCreatedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    {
        auto& addCog = commandAdd.ensure<xg::command::AddCogComponent>();
        addCog.m_CogId = s_TestCog1;
        addCog.m_Position = glm::ivec2(1, 2);
        addCog.m_Rotation = xc::Rotation90(1);
    }
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();

    env.Update();

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::CogCreatedComponent>() == false);
}