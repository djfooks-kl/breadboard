#include "BreadTest.h"
#include <flecs/flecs.h>

#include "UIPreviewWireSystem.h"
#include "UIPreviewAddingWireComponent.h"
#include "UIPreviewWireComponent.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIPreviewWireSystem - " description, "[xg::UIPreviewWireSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::UIPreviewAddingWireComponent>();
        }

        void Update()
        {
            xg::UIPreviewWireSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("When starting a wire preview -> start position is rounded and stored in a new UIPreviewWireComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UIPreviewAddingWireComponent>().m_Active = true;
    world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(1, 2);

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.count<xg::UIPreviewWireComponent>() == 1);
    world.each([&](flecs::entity entity, const xg::UIPreviewWireComponent& preview)
        {
            createdEntity = entity;
            CHECK(preview.m_Start == glm::ivec2(1, 2));
        });
}

SYSTEM_TEST_CASE("After starting preview and moving the mouse -> start position is not changed (and only 1 preview is created)")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UIPreviewAddingWireComponent>().m_Active = true;
    world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(3, 4);

    env.Update();

    world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(9, 9);
    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.count<xg::UIPreviewWireComponent>() == 1);
    world.each([&](flecs::entity entity, const xg::UIPreviewWireComponent& preview)
        {
            createdEntity = entity;
            CHECK(preview.m_Start == glm::ivec2(3, 4));
        });
}

SYSTEM_TEST_CASE("Adding is no longer active -> destroy the preview entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UIPreviewAddingWireComponent>().m_Active = true;
    env.Update();

    flecs::entity createdEntity;
    world.each([&](flecs::entity entity, const xg::UIPreviewWireComponent&)
        {
            createdEntity = entity;
        });

    world.get_mut<xg::UIPreviewAddingWireComponent>().m_Active = false;
    env.Update();

    CHECK(createdEntity.is_alive() == false);
}