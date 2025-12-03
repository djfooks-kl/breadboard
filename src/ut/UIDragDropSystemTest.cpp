#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "UIAddCogComponent.h"
#include "UIDragDropSystem.h"
#include "UIDraggingDropComponent.h"
#include "UIDragPreviewComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::ui_drag_drop_system - " description, "[xg::ui_drag_drop_system]")

namespace
{
    static const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");
    static const xg::CogResourceId s_TestCog2 = xg::CogResourceId::Create("TestCog2");

    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::UIDraggingDropComponent>();
        }

        void Update()
        {
            xg::UIDragDropSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("When dropping add a new entity requesting the cog at the given position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& dragPreview = entity.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_TestCog1;
        dragPreview.m_Position = glm::ivec2(1, 2);
        dragPreview.m_Rotation = xc::Rotation90(3);
    }
    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = true;

    env.Update();

    REQUIRE(world.count<xg::UIAddCogComponent>() == 1);

    world.each([&](flecs::entity newEntity, const xg::UIAddCogComponent& addCog)
        {
            CHECK(newEntity != entity);

            CHECK(addCog.m_CogId == s_TestCog1);
            CHECK(addCog.m_Position == glm::ivec2(1, 2));
        });
}

SYSTEM_TEST_CASE("After dropping remove the UIAddCogComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& dragPreview = entity.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_TestCog1;
        dragPreview.m_Position = glm::ivec2(1, 2);
        dragPreview.m_Rotation = xc::Rotation90(3);
    }
    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = true;

    env.Update();

    REQUIRE(world.count<xg::UIAddCogComponent>() == 1);

    world.each([&](flecs::entity newEntity, const xg::UIAddCogComponent& addCog)
        {
            CHECK(newEntity != entity);

            CHECK(addCog.m_CogId == s_TestCog1);
            CHECK(addCog.m_Position == glm::ivec2(1, 2));
        });

    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = false;
    env.Update();

    CHECK(world.count<xg::UIAddCogComponent>() == 0);
}

SYSTEM_TEST_CASE("When not dropping do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    {
        auto& dragPreview = entity.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_TestCog1;
        dragPreview.m_Position = glm::ivec2(1, 2);
        dragPreview.m_Rotation = xc::Rotation90(3);
    }
    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = false;

    env.Update();

    CHECK(world.count<xg::UIAddCogComponent>() == 0);
}
