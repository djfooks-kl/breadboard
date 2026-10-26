#include "BreadTest.h"
#include <flecs/flecs.h>

#include "Core/Range.h"
#include "UIAddCogComponent.h"
#include "UIDragDeleteGroupComponent.h"
#include "UIDragDropSystem.h"
#include "UIDraggingDropComponent.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIDragValidComponent.h"
#include "UIMoveGroupComponent.h"
#include "UIPreviewMovingComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIDragDropSystem - " description, "[xg::UIDragDropSystem]")

namespace
{
    static const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");
    static const xg::CogResourceId s_TestCog2 = xg::CogResourceId::Create("TestCog2");

    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::UIDraggingDropComponent>();
            m_World.ensure<xg::UIDragPreviewMovement>();
            m_World.ensure<xg::UIDragValidComponent>().m_Valid = true;
            m_World.ensure<xg::UIPreviewMovingComponent>();
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
    entity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(1, 2);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(3);

    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = true;

    env.Update();

    REQUIRE(world.count<xg::UIAddCogComponent>() == 1);
    CHECK(world.count<xg::UIMoveGroupComponent>() == 0);
    CHECK(world.count<xg::UIDragDeleteGroupComponent>() == 0);

    world.each([&](flecs::entity newEntity, const xg::UIAddCogComponent& addCog)
        {
            CHECK(newEntity != entity);

            CHECK(addCog.m_CogId == s_TestCog1);
            CHECK(addCog.m_Transform == xc::ITransform{ glm::ivec2(1, 2), xc::Rotation90(3) });
        });
}

SYSTEM_TEST_CASE("After dropping destroy the UIAddCogComponent entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    entity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(1, 2);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(3);

    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = true;

    env.Update();

    REQUIRE(world.count<xg::UIAddCogComponent>() == 1);

    flecs::entity newEntity;
    world.each([&](flecs::entity addCogEntity, const xg::UIAddCogComponent& addCog)
        {
            newEntity = addCogEntity;
            CHECK(addCogEntity != entity);

            CHECK(addCog.m_CogId == s_TestCog1);
            CHECK(addCog.m_Transform == xc::ITransform{ glm::ivec2(1, 2), xc::Rotation90(3) });
        });

    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = false;
    env.Update();

    CHECK(world.count<xg::UIAddCogComponent>() == 0);
    CHECK(world.count<xg::UIMoveGroupComponent>() == 0);
    CHECK(world.count<xg::UIDragDeleteGroupComponent>() == 0);
    CHECK(newEntity.is_alive() == false);
}

SYSTEM_TEST_CASE("When not dropping an invalid cog do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    entity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(1, 2);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(3);

    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = false;

    env.Update();

    CHECK(world.count<xg::UIAddCogComponent>() == 0);
    CHECK(world.count<xg::UIMoveGroupComponent>() == 0);
    CHECK(world.count<xg::UIDragDeleteGroupComponent>() == 0);
}

SYSTEM_TEST_CASE("When dropping an added cog but it is invalid do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    entity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(1, 2);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(3);

    world.get_mut<xg::UIDraggingDropComponent>().m_Drop = true;
    world.get_mut<xg::UIDragValidComponent>().m_Valid = false;

    env.Update();

    CHECK(world.count<xg::UIAddCogComponent>() == 0);
    CHECK(world.count<xg::UIMoveGroupComponent>() == 0);
    CHECK(world.count<xg::UIDragDeleteGroupComponent>() == 0);
}

SYSTEM_TEST_CASE("When dropping a valid moved group -> add a new entity requesting the move, then next frame remove it")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog1 = world.entity();
    flecs::entity cog2 = world.entity();

    cog1.add<xg::UIDragPreviewComponent>();
    cog2.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(1, 2);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(3);

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = false;
    world.get_mut<xg::UIDragValidComponent>().m_Valid = true;
    env.Update();

    REQUIRE(world.count<xg::UIMoveGroupComponent>() == 1);

    flecs::entity added;
    world.each([&](flecs::entity newEntity, const xg::UIMoveGroupComponent& moveGroup)
        {
            added = newEntity;
            CHECK(newEntity != cog1);
            CHECK(newEntity != cog2);

            CHECK(xc::RangeContains(moveGroup.m_Entities, cog1));
            CHECK(xc::RangeContains(moveGroup.m_Entities, cog2));
            CHECK(moveGroup.m_Entities.size() == 2);
            CHECK(moveGroup.m_Transform == xc::ITransform{ glm::ivec2(1, 2), xc::Rotation90(3) });
        });

    cog1.remove<xg::UIDragPreviewComponent>();
    cog2.remove<xg::UIDragPreviewComponent>();
    env.Update();

    CHECK(world.count<xg::UIMoveGroupComponent>() == 0);
    CHECK(world.is_alive(added) == false);
}

SYSTEM_TEST_CASE("When dropping an invalid moved group -> add a new entity requesting the delete, then next frame remove it")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog1 = world.entity();
    flecs::entity cog2 = world.entity();

    cog1.add<xg::UIDragPreviewComponent>();
    cog2.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(1, 2);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(3);

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = false;
    world.get_mut<xg::UIDragValidComponent>().m_Valid = false;
    env.Update();

    REQUIRE(world.count<xg::UIDragDeleteGroupComponent>() == 1);
    CHECK(world.count<xg::UIMoveGroupComponent>() == 0);

    flecs::entity added;
    world.each([&](flecs::entity newEntity, const xg::UIDragDeleteGroupComponent& moveGroup)
        {
            added = newEntity;
            CHECK(newEntity != cog1);
            CHECK(newEntity != cog2);

            CHECK(xc::RangeContains(moveGroup.m_Entities, cog1));
            CHECK(xc::RangeContains(moveGroup.m_Entities, cog2));
            CHECK(moveGroup.m_Entities.size() == 2);
        });

    cog1.remove<xg::UIDragPreviewComponent>();
    cog2.remove<xg::UIDragPreviewComponent>();
    env.Update();

    CHECK(world.count<xg::UIDragDeleteGroupComponent>() == 0);
    CHECK(world.count<xg::UIMoveGroupComponent>() == 0);
    CHECK(world.is_alive(added) == false);
}
