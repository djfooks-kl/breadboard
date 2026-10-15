#include "BreadTest.h"

#include <catch2/catch_template_test_macros.hpp>
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "GridAttachmentsComponent.h"
#include "GridSizeComponent.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIDragValidationSystem.h"
#include "UIDragValidComponent.h"
#include "WireComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIDragValidationSystem - " description, "[xg::UIDragValidationSystem]")

#define SYSTEM_TEMPLATE_TEST_CASE(description, ...)  \
    TEMPLATE_TEST_CASE(                              \
        "xg::UIDragValidationSystem - " description, \
        "[xg::UIDragValidationSystem]",              \
        __VA_ARGS__)

namespace
{
    static const xg::CogResourceId s_Size1Cog = xg::CogResourceId::Create("Size1Cog");
    static const xg::CogResourceId s_LongCog = xg::CogResourceId::Create("LongCog");

    struct Size1Cog final : public xg::CogPrototype
    {
        virtual xg::CogResourceId GetResourceId() const { return s_Size1Cog; }

        virtual glm::ivec2 GetSize() const { return glm::ivec2(1, 1); }
    };

    struct LongCog final : public xg::CogPrototype
    {
        virtual xg::CogResourceId GetResourceId() const { return s_LongCog; }

        virtual glm::ivec2 GetSize() const { return glm::ivec2(2, 3); }
    };

    struct TestEnv
    {
        TestEnv()
        {
            m_World.emplace<xg::GridAttachmentsComponent>();
            m_World.emplace<xg::UIDragValidComponent>();
            m_World.ensure<xg::GridSizeComponent>().m_Size = glm::ivec2(1000, 1000);
            m_World.ensure<xg::UIDragPreviewMovement>();

            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<Size1Cog>();
            cogMap.Register<LongCog>();
        }

        void Update()
        {
            xg::UIDragValidationSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with nothing on grid -> Preview valid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_Size1Cog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 5);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with a cog on the grid in the same place -> Preview invalid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_Size1Cog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 4);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(3, 4)].m_Entities.push_back(gridCog);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with a cog on the grid in a different place -> Preview valid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_Size1Cog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 4);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(10, 5)].m_Entities.push_back(gridCog);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with a non-cog on the grid in the same place -> Preview valid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_Size1Cog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 5);

    flecs::entity notACog = world.entity();

    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(3, 5)].m_Entities.push_back(notACog);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with cog on the grid overlapping -> Preview invalid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 5);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map[glm::ivec2(3, 5)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 5)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(3, 6)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 6)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(3, 7)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 7)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with cog on the grid not overlapping -> Preview valid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 5);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map[glm::ivec2(2, 5)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(5, 5)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(3, 8)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(3, 4)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with cog on the grid overlapping with the rotated cog -> Preview invalid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 2);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(1);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map.clear();
    map[glm::ivec2(3, 2)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 2)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(5, 2)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);


    map.clear();
    map[glm::ivec2(3, 1)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 1)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(5, 1)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog with cog on the grid not overlapping with the rotated cog -> Preview valid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::vec2(3, 5);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(1);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map.clear();
    map[glm::ivec2(3, 0)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(4, 3)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(2, 1)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(6, 1)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog goes into x<0 -> Preview invalid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(0, 0);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(-1);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog goes into y<0 -> Preview invalid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(0, 0);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(1);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Cog goes into x==gridSize -> Preview valid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(9, 0);

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(10, 300);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog is x > gridSize -> Preview invalid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(100, 0);

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(10, 300);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Cog goes into y==gridSize -> Preview valid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(0, 8);

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(300, 10);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEMPLATE_TEST_CASE(
    "Preview cog is y > gridSize -> Preview invalid",
    xg::UIDragPreviewCogComponent,
    xg::CogComponent)
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<TestType>().m_CogId = s_LongCog;
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(0, 100);

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(300, 10);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE(
    "Preview cog already has a translate -> Preview valid when not overlapping")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<xg::CogComponent>().m_CogId = s_Size1Cog;
    draggingCog.ensure<xg::CogComponent>().m_Transform.m_Translation = glm::ivec2(10, 20);
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 4);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(13, 24)].m_Entities.push_back(gridCog);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    draggingCog.ensure<xg::CogComponent>().m_Transform.m_Translation = glm::ivec2(9, 20);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE(
    "Preview cog already has a rotation -> Preview valid when not overlapping")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    draggingCog.ensure<xg::CogComponent>().m_CogId = s_LongCog;
    draggingCog.ensure<xg::CogComponent>().m_Transform.m_Translation = glm::ivec2(10, 2);
    draggingCog.ensure<xg::CogComponent>().m_Transform.m_Rotation = xc::Rotation90(1);
    draggingCog.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(3, 23);
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(1);

    flecs::entity gridCog = world.entity();
    gridCog.ensure<xg::CogComponent>();

    // p1 = (5, 13)
    // p2 = (5, 13) - (1, 2) = (4, 11)

    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(5, 13)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    world.get_mut<xg::GridAttachmentsComponent>().m_Map.clear();
    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(4, 11)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    world.get_mut<xg::GridAttachmentsComponent>().m_Map.clear();
    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(3, 11)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    world.get_mut<xg::GridAttachmentsComponent>().m_Map.clear();
    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(4, 14)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    world.get_mut<xg::GridAttachmentsComponent>().m_Map.clear();
    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(6, 11)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    world.get_mut<xg::GridAttachmentsComponent>().m_Map.clear();
    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(4, 10)].m_Entities.push_back(gridCog);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("Preview wire goes off grid -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;
    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(100, 100);

    flecs::entity draggingWire = world.entity();
    draggingWire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 1));
    draggingWire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
    draggingWire.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(1);
    // after rotation will be at (1, -1)->(2, -1)

    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(-3, 2);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(0, 0);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(99, 2);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(2, 102);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("Preview wire goes just inside grid -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;
    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(100, 100);

    flecs::entity draggingWire = world.entity();
    draggingWire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 1));
    draggingWire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
    draggingWire.add<xg::UIDragPreviewComponent>();
    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Rotation = xc::Rotation90(1);
    // after rotation will be at (1, -1)->(2, -1)

    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(-1, 1);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    world.get_mut<xg::UIDragPreviewMovement>().m_Transform.m_Translation = glm::ivec2(98, 101);
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}