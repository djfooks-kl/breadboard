#include <catch2/catch_test_macros.hpp>
#include <flecs/flecs.h>

#include "Cogs/CogMap.h"
#include "GridAttachmentsComponent.h"
#include "GridSizeComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragValidationSystem.h"
#include "UIDragValidComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIDragValidationSystem - " description, "[xg::UIDragValidationSystem]")

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

SYSTEM_TEST_CASE("Nothing on grid -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_Size1Cog;
        dragPreview.m_Position = glm::ivec2(3, 5);
    }

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("Something on the grid in the same place -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_Size1Cog;
        dragPreview.m_Position = glm::ivec2(3, 5);
    }

    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(3, 5)];

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("Something on the grid in a different place -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_Size1Cog;
        dragPreview.m_Position = glm::ivec2(3, 5);
    }

    world.get_mut<xg::GridAttachmentsComponent>().m_Map[glm::ivec2(10, 5)];

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("Something on the grid overlapping with the cog -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(3, 5);
    }

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map[glm::ivec2(3, 5)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 5)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(3, 6)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 6)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(3, 7)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 7)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("Something on the grid not overlapping with the cog -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(3, 5);
    }

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map[glm::ivec2(2, 5)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(5, 5)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(3, 8)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(3, 4)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("Something on the grid overlapping with the rotated cog -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(3, 2);
        dragPreview.m_Rotation = xc::Rotation90(1);
    }

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map.clear();
    map[glm::ivec2(3, 2)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 2)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(5, 2)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);


    map.clear();
    map[glm::ivec2(3, 1)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(4, 1)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);

    map.clear();
    map[glm::ivec2(5, 1)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("Something on the grid not overlapping with the rotated cog -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(3, 2);
        dragPreview.m_Rotation = xc::Rotation90(1);
    }

    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    map.clear();
    map[glm::ivec2(3, 0)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(4, 3)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(2, 1)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);

    map.clear();
    map[glm::ivec2(6, 1)];
    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("Cog goes into x<0 -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(0, 0);
        dragPreview.m_Rotation = xc::Rotation90(-1);
    }

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("Cog goes into y<0 -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(0, 0);
        dragPreview.m_Rotation = xc::Rotation90(1);
    }

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("Cog goes into x==gridSize -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(9, 0);
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(10, 300);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("Cog is x > gridSize -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(100, 0);
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(10, 300);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}

SYSTEM_TEST_CASE("Cog goes into y==gridSize -> Preview valid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(0, 8);
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(300, 10);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == true);
}

SYSTEM_TEST_CASE("Cog is y > gridSize -> Preview invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity draggingCog = world.entity();
    {
        auto& dragPreview = draggingCog.ensure<xg::UIDragPreviewComponent>();
        dragPreview.m_CogId = s_LongCog;
        dragPreview.m_Position = glm::ivec2(0, 100);
    }

    world.get_mut<xg::GridSizeComponent>().m_Size = glm::ivec2(300, 10);

    env.Update();
    CHECK(world.get<xg::UIDragValidComponent>().m_Valid == false);
}
