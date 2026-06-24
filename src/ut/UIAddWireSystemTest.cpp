#include "BreadTest.h"
#include <flecs/flecs.h>

#include "UIAddWireComponent.h"
#include "UIPreviewCreateWireComponent.h"
#include "UIAddWireSystem.h"
#include "UIWireSegmentsComponent.h"
#include "UIWireValidComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIAddWireSystem - " description, "[xg::UIAddWireSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
    {
            m_World.ensure<xg::UIPreviewCreateWireComponent>().m_Create = false;
        }

        void Update()
        {
            xg::UIAddWireSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Adding a valid preview wire -> Add a xg::UIAddWireComponent on a new entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    previewWire.ensure<xg::UIWireValidComponent>().m_Valid = true;
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.ensure<xg::UIPreviewCreateWireComponent>().m_Create = true;
    env.Update();

    REQUIRE(world.count<xg::UIAddWireComponent>() == 1);

    world.each([&](flecs::entity newEntity, const xg::UIAddWireComponent& addWire)
        {
            CHECK(newEntity != previewWire);

            REQUIRE(addWire.m_Checkpoints.size() == 2);
            CHECK(addWire.m_Checkpoints[0] == glm::ivec2(0, 0));
            CHECK(addWire.m_Checkpoints[1] == glm::ivec2(1, 1));
        });
}

SYSTEM_TEST_CASE("When not creating a wire do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    previewWire.ensure<xg::UIWireValidComponent>().m_Valid = true;
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.ensure<xg::UIPreviewCreateWireComponent>().m_Create = false;
    env.Update();

    CHECK(world.count<xg::UIAddWireComponent>() == 0);
}

SYSTEM_TEST_CASE("After adding destroy the UIAddWireComponent entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    previewWire.ensure<xg::UIWireValidComponent>().m_Valid = true;
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.ensure<xg::UIPreviewCreateWireComponent>().m_Create = true;
    env.Update();

    world.get_mut<xg::UIPreviewCreateWireComponent>().m_Create = false;
    env.Update();

    CHECK(world.count<xg::UIAddWireComponent>() == 0);
}

SYSTEM_TEST_CASE("When creating but it is invalid do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity previewWire = world.entity();
    previewWire.ensure<xg::UIWireValidComponent>().m_Valid = false;
    {
        auto& wire = previewWire.ensure<xg::UIWireSegmentsComponent>();
        wire.m_Checkpoints.push_back(glm::ivec2(0, 0));
        wire.m_Checkpoints.push_back(glm::ivec2(1, 1));
    }

    world.ensure<xg::UIPreviewCreateWireComponent>().m_Create = true;
    env.Update();

    CHECK(world.count<xg::UIAddWireComponent>() == 0);
}
