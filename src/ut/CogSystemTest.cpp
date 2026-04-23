#include "BreadTest.h"
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "CogCreatedComponent.h"
#include "CogNodesComponent.h"
#include "Cogs/CogMap.h"
#include "CogSystem.h"
#include "Command/CommandAddCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::CogSystem - " description, "[xg::CogSystem]")

namespace
{
    const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");
    const std::vector<glm::ivec2> s_WireNodes{ glm::ivec2(0, 0), glm::ivec2(2, 0)};

    struct TestCog1 final : public xg::CogPrototype
    {
        xg::CogResourceId GetResourceId() const override { return s_TestCog1; }

        glm::ivec2 GetSize() const override { return glm::ivec2(3, 1); }

        const std::vector<glm::ivec2>& GetWireNodes() const override { return s_WireNodes; }
    };

    struct TestEnv
    {
        TestEnv()
        {
            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<TestCog1>();
        }

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

SYSTEM_TEST_CASE("Executing an add command -> "
    "Copy the added values onto the CogComponent, add the CogNodesComponent and add a CogCreatedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity addedEntity = world.entity();
    flecs::entity commandAdd = world.entity();
    {
        auto& addCog = commandAdd.ensure<xg::command::AddCogComponent>();
        addCog.m_CogId = s_TestCog1;
        addCog.m_Transform = xc::ITransform{ glm::ivec2(1, 2), xc::Rotation90(1) };
    }
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();

    env.Update();
    CHECK(addedEntity.has<xg::CogCreatedComponent>());
    REQUIRE(addedEntity.has<xg::CogComponent>());
    CHECK(addedEntity.get<xg::CogComponent>().m_CogId == s_TestCog1);
    CHECK(addedEntity.get<xg::CogComponent>().m_Transform == xc::ITransform{ glm::ivec2(1, 2), xc::Rotation90(1) });

    REQUIRE(addedEntity.has<xg::CogNodesComponent>());
    REQUIRE(addedEntity.get<xg::CogNodesComponent>().m_Nodes.size() == 2);
    CHECK(addedEntity.get<xg::CogNodesComponent>().m_Nodes[0] == glm::ivec2(1, 2));
    CHECK(addedEntity.get<xg::CogNodesComponent>().m_Nodes[1] == glm::ivec2(1, 0));

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
        addCog.m_Transform = xc::ITransform{ glm::ivec2(1, 2), xc::Rotation90(1) };
    }
    commandAdd.ensure<xg::command::EntityComponent>().m_Entity = addedEntity;
    commandAdd.add<xg::command::ExecuteComponent>();

    env.Update();

    commandAdd.add<xg::command::ExecuteComponent>();
    env.Update();

    CHECK(addedEntity.has<xg::CogCreatedComponent>() == false);
}