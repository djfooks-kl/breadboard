#include "BreadTest.h"
#include <flecs/flecs.h>

#include "GridSizeComponent.h"
#include "UIWireSegmentsComponent.h"
#include "UIWireValidationSystem.h"
#include "UIWireValidComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIWireValidationSystem - " description, "[xg::UIWireValidationSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::GridSizeComponent>().m_Size = glm::ivec2(1000, 1000);
        }

        void Update()
        {
            xg::UIWireValidationSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Inside the grid -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    env.Update();
    REQUIRE(previewWire.has<xg::UIWireValidComponent>());
    CHECK(previewWire.get<xg::UIWireValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("A segment goes into x<0 -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(-1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    env.Update();
    REQUIRE(previewWire.has<xg::UIWireValidComponent>());
    CHECK(previewWire.get<xg::UIWireValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("A segment goes y<0 -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(1, -1));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    env.Update();
    REQUIRE(previewWire.has<xg::UIWireValidComponent>());
    CHECK(previewWire.get<xg::UIWireValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("A segment goes x==gridSize -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(10, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(10, 300);

    env.Update();
    REQUIRE(previewWire.has<xg::UIWireValidComponent>());
    CHECK(previewWire.get<xg::UIWireValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("A segment goes x > gridSize -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(11, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(10, 300);

    env.Update();
    REQUIRE(previewWire.has<xg::UIWireValidComponent>());
    CHECK(previewWire.get<xg::UIWireValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("A segment goes y==gridSize -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 10));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(300, 10);

    env.Update();
    REQUIRE(previewWire.has<xg::UIWireValidComponent>());
    CHECK(previewWire.get<xg::UIWireValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("A segment goes y > gridSize -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 11));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(300, 10);

    env.Update();
    REQUIRE(previewWire.has<xg::UIWireValidComponent>());
    CHECK(previewWire.get<xg::UIWireValidComponent>().m_Valid == false);
}
