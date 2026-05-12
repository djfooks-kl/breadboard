#include "BreadTest.h"
#include <flecs/flecs.h>

#include "GridAttachmentsComponent.h"
#include "RenderSettings.h"
#include "UIHoverComponent.h"
#include "UIHoverSystem.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIHoverSystem - " description, "[xg::UIHoverSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::GridAttachmentsComponent>();
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::UIHoverComponent>();
            m_World.ensure<xg::RenderSettings>().m_NodeOuterRadius = 0.25f;
        }

        void Update()
        {
            xg::UIHoverSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("While hovering a wire node -> set the node flag to true")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasNode = true;

        world.get_mut<xg::RenderSettings>().m_NodeOuterRadius = 100.f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Node);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.51f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Node);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.49f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Node);
}

SYSTEM_TEST_CASE("While not hovering a wire node cell -> set the node flag to false")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasNode = true;

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.f, 2.f);
    }

    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Node == false);
}

SYSTEM_TEST_CASE("While hovering a wire node cell but not the wire node circle -> set the node flag to false")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasNode = true;

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.26f, 2.f);
    }

    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Node == false);
}