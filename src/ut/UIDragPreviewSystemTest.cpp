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
            m_World.emplace<xg::UIRotateComponent>();
            m_World.ensure<xg::WorldMouseComponent>();
        }

        void Update()
        {
            xg::ui_drag_preview_system::Update(m_World);

            m_World.get_mut<xg::UIRotateComponent>().m_RotationDirection = 0;
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("While hovering -> show a preview at the given position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiPreview = world.entity();
    {
        auto& uiPreviewAddingCog = uiPreview.ensure<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_CogId = s_TestCog1;
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(1.f, 2.f);
        uiPreviewAddingCog.m_Hovering = true;
    }

    env.Update();

    REQUIRE(uiPreview.has<xg::UIDragPreviewComponent>());
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_CogId == s_TestCog1);
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_Rotation == xc::Rotation90(0));
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(0, 0));
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(1.f, 2.f));
}

SYSTEM_TEST_CASE("Hovering ends -> show a preview at the mouse position instead")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiPreview = world.entity();
    {
        auto& uiPreviewAddingCog = uiPreview.ensure<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_CogId = s_TestCog1;
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(1.f, 2.f);
        uiPreviewAddingCog.m_Hovering = true;
    }
    env.Update();
    {
        auto& uiPreviewAddingCog = uiPreview.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_Hovering = false;

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(3.f, 4.f);
    }
    env.Update();

    REQUIRE(uiPreview.has<xg::UIDragPreviewComponent>());
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_CogId == s_TestCog1);
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_Rotation == xc::Rotation90(0));
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(3, 4));
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(3.f, 4.f));

    {
        auto& uiPreviewAddingCog = uiPreview.get_mut<xg::UIPreviewAddingCogComponent>();
        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(5.1f, 6.1f);
    }
    env.Update();

    REQUIRE(uiPreview.has<xg::UIDragPreviewComponent>());
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(5, 6));
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(5.1f, 6.1f));

    {
        auto& uiPreviewAddingCog = uiPreview.get_mut<xg::UIPreviewAddingCogComponent>();
        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(7.9f, 8.9f);
    }
    env.Update();

    REQUIRE(uiPreview.has<xg::UIDragPreviewComponent>());
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_Position == glm::ivec2(8, 9));
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_PreviewPosition == glm::vec2(7.9f, 8.9f));

}

SYSTEM_TEST_CASE("Rotation changes while previewing -> Update the rotation")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity uiPreview = world.entity();
    {
        auto& uiPreviewAddingCog = uiPreview.ensure<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_Hovering = false;

        world.get_mut<xg::UIRotateComponent>().m_RotationDirection = 1;
    }
    env.Update();

    REQUIRE(uiPreview.has<xg::UIDragPreviewComponent>());
    CHECK(uiPreview.get<xg::UIDragPreviewComponent>().m_Rotation == xc::Rotation90(1));
}
