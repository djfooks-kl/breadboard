#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "UIDragPreviewComponent.h"
#include "UIDragPreviewSystem.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIRotateComponent.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::preview_system - " description, "[xg::preview_system]")

namespace
{
    static const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");
    static const xg::CogResourceId s_TestCog2 = xg::CogResourceId::Create("TestCog2");

    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::UIRotateComponent>();
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::UIPreviewAddingCogComponent>();
        }

        void Update()
        {
            xg::UIDragPreviewSystem::Update(m_World);

            m_World.get_mut<xg::UIRotateComponent>().m_RotationDirection = 0;
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("While hovering -> show a preview at the given position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = s_TestCog1;
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(1.f, 2.f);
    }

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.count<xg::UIDragPreviewComponent>() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent& dragPreview)
        {
            createdEntity = entity;
            CHECK(dragPreview.m_CogId == s_TestCog1);
            CHECK(dragPreview.m_Rotation == xc::Rotation90(0));
            CHECK(dragPreview.m_Position == glm::ivec2(0, 0));
            CHECK(dragPreview.m_PreviewPosition == glm::vec2(1.f, 2.f));
        });

    env.Update();

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(2.f, 3.f);
    }

    env.Update();

    REQUIRE(world.count<xg::UIDragPreviewComponent>() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent& dragPreview)
        {
            CHECK(createdEntity == entity);
            CHECK(dragPreview.m_CogId == s_TestCog1);
            CHECK(dragPreview.m_Rotation == xc::Rotation90(0));
            CHECK(dragPreview.m_Position == glm::ivec2(0, 0));
            CHECK(dragPreview.m_PreviewPosition == glm::vec2(2.f, 3.f));
        });
}

SYSTEM_TEST_CASE("Stop hovering -> remove the drag preview")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = s_TestCog1;
    }

    env.Update();

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = xg::CogResourceId();
    }

    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 0);
}

SYSTEM_TEST_CASE("Hovering ends and cog is being added -> show a preview at the mouse position instead")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = s_TestCog1;
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(1.f, 2.f);
    }

    env.Update();

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = xg::CogResourceId();
        uiPreviewAddingCog.m_AddCogId = s_TestCog1;
    }
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(3.f, 4.f);

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.count<xg::UIDragPreviewComponent>() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent& dragPreview)
        {
            createdEntity = entity;
            CHECK(dragPreview.m_CogId == s_TestCog1);
            CHECK(dragPreview.m_Rotation == xc::Rotation90(0));
            CHECK(dragPreview.m_Position == glm::ivec2(3, 4));
            CHECK(dragPreview.m_PreviewPosition == glm::vec2(3.f, 4.f));
        });

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(5.1f, 6.1f);
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    REQUIRE(createdEntity.has<xg::UIDragPreviewComponent>());
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(5, 6));
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(5.1f, 6.1f));

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(7.9f, 8.9f);
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    REQUIRE(createdEntity.has<xg::UIDragPreviewComponent>());
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(8, 9));
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(7.9f, 8.9f));
}

SYSTEM_TEST_CASE("Cog is being added (without hovering first) -> show a preview at the mouse position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_AddCogId = s_TestCog1;
    }
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(3.f, 4.f);

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.count<xg::UIDragPreviewComponent>() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent& dragPreview)
        {
            createdEntity = entity;
            CHECK(dragPreview.m_CogId == s_TestCog1);
            CHECK(dragPreview.m_Rotation == xc::Rotation90(0));
            CHECK(dragPreview.m_Position == glm::ivec2(3, 4));
            CHECK(dragPreview.m_PreviewPosition == glm::vec2(3.f, 4.f));
        });

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(5.1f, 6.1f);
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    REQUIRE(createdEntity.has<xg::UIDragPreviewComponent>());
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(5, 6));
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(5.1f, 6.1f));

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(7.9f, 8.9f);
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    REQUIRE(createdEntity.has<xg::UIDragPreviewComponent>());
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(8, 9));
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(7.9f, 8.9f));
}

SYSTEM_TEST_CASE("Rotation changes while previewing -> Update the rotation")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_AddCogId = s_TestCog1;
    }

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.count<xg::UIDragPreviewComponent>() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&)
        {
            createdEntity = entity;
        });

    world.get_mut<xg::UIRotateComponent>().m_RotationDirection = 1;
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    REQUIRE(createdEntity.has<xg::UIDragPreviewComponent>());
    CHECK(createdEntity.get<xg::UIDragPreviewComponent>().m_Rotation == xc::Rotation90(1));
}
