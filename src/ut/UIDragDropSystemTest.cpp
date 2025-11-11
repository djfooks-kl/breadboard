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
        }

        void Update()
        {
            xg::ui_drag_drop_system::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("While hovering -> show a preview at the given position")
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
    entity.add<xg::UIDraggingDropComponent>();

    env.Update();

    REQUIRE(entity.has<xg::UIAddCogComponent>());
    CHECK(entity.get<xg::UIAddCogComponent>().m_CogId == s_TestCog1);
    CHECK(entity.get<xg::UIAddCogComponent>().m_Position == glm::ivec2(1, 2));
}
