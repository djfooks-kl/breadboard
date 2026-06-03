#include "BreadTest.h"
#include <flecs/flecs.h>

#include "UIWirePathfindingSystem.h"
#include "UIPreviewWireComponent.h"
#include "UIWirePathComponent.h"
#include "WorldMouseComponent.h"
#include "UISettings.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIWirePathfindingSystem - " description, "[xg::UIWirePathfindingSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::UISettings>().m_WireDirectionHintRadius = 0.f;
        }

        void Update()
        {
            xg::UIWirePathfindingSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Start=end, output just the start cell")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(1, 2);

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(1, 2);

    env.Update();

    REQUIRE(preview.has<xg::UIWirePathComponent>());
    REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 1);
    CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(1, 2));
}

SYSTEM_TEST_CASE("Start and end are just 1 cell apart, output both start and end")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);

    world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(11, 10);

    env.Update();

    REQUIRE(preview.has<xg::UIWirePathComponent>());
    REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 2);
    CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
    CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(11, 10));
}

SYSTEM_TEST_CASE("Mouse moves -> Update with new path")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);

    world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(11, 10);

    env.Update();

    world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(11, 11);
    env.Update();

    REQUIRE(preview.has<xg::UIWirePathComponent>());
    REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 2);
    CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
    CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(11, 11));
}

SYSTEM_TEST_CASE("If m_StartDirection not yet set -> Set m_StartDirection to be the current direction")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        flecs::entity preview = world.entity();
        preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(11.f, 10.f);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 0));
    }
}

SYSTEM_TEST_CASE("If within WireDirectionHintRadius -> Set m_StartDirection to be the current direction")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISettings>().m_WireDirectionHintRadius = 100.f;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);
    env.Update();

    {
        const glm::vec2 mouse = glm::vec2(11.f, 10.f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 0));
    }
    {
        const glm::vec2 mouse = glm::vec2(10.f, 11.f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(0, 1));
    }
    {
        const glm::vec2 mouse = glm::vec2(10.f, 9.f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(0, -1));
    }
    {
        const glm::vec2 mouse = glm::vec2(9.f, 10.f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(-1, 0));
    }
    {
        const glm::vec2 mouse = glm::vec2(10.5f, 10.5f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 1));
    }
    {
        const glm::vec2 mouse = glm::vec2(10.5f, 9.5f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, -1));
    }
    {
        const glm::vec2 mouse = glm::vec2(9.5f, 10.5f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(-1, 1));
    }
    {
        const glm::vec2 mouse = glm::vec2(9.5f, 9.5f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(-1, -1));
    }
    {
        const glm::vec2 mouse = glm::vec2(11.f, 10.01f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 0));
    }
    {
        const glm::vec2 mouse = glm::vec2(10.f + sqrt(3.f) - 0.01f, 11.f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 1));
    }
    {
        const glm::vec2 mouse = glm::vec2(10.f + sqrt(3.f) + 0.01f, 11.f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 0));
    }
    {
        const glm::vec2 mouse = glm::vec2(11.f, 10.f + sqrt(3.f) - 0.01f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 1));
    }
    {
        const glm::vec2 mouse = glm::vec2(11.f, 10.f + sqrt(3.f) + 0.01f);
        INFO(mouse);
        world.get_mut<xg::WorldMouseComponent>().m_Position = mouse;
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(0, 1));
    }
}

SYSTEM_TEST_CASE("If at the border of WireDirectionHintRadius distance -> "
    "Only change StartDirection at or below WireDirectionHintRadius distance")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISettings>().m_WireDirectionHintRadius = 2.f;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);
    env.Update();

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(11.f, 10.f);
    env.Update();

    {
        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.f, 10.f);
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 0));
    }
    {
        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(7.99f, 10.f);
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(1, 0));
    }
    {
        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(8.f, 10.f);
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(-1, 0));
    }
    {
        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(10.f, 11.99f);
        env.Update();
        CHECK(preview.get<xg::UIWirePathComponent>().m_StartDirection == glm::ivec2(0, 1));
    }
}

SYSTEM_TEST_CASE("Start and end are on a straight line path -> Output line from start to end")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);

    {
        world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(15, 10);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 6);
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(11, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[2] == glm::ivec2(12, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[3] == glm::ivec2(13, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[4] == glm::ivec2(14, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[5] == glm::ivec2(15, 10));
    }

    {
        world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(7, 7);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 4);
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(9, 9));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[2] == glm::ivec2(8, 8));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[3] == glm::ivec2(7, 7));
    }
}

SYSTEM_TEST_CASE("Start and end direction=StartDirection but are not on a straight line -> "
    "Output path from start to end initially using the StartDirection")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISettings>().m_WireDirectionHintRadius = 2.f;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(11.f, 10.f);
    env.Update();

    {
        world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(15, 11);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 6);
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(11, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[2] == glm::ivec2(12, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[3] == glm::ivec2(13, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[4] == glm::ivec2(14, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[5] == glm::ivec2(15, 11));
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(9.f, 9.f);
    env.Update();

    {
        world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(7, 5);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 6);
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(9, 9));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[2] == glm::ivec2(8, 8));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[3] == glm::ivec2(7, 7));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[4] == glm::ivec2(7, 6));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[5] == glm::ivec2(7, 5));
    }
}

SYSTEM_TEST_CASE("Start and end direction is opposite to StartDirection -> "
    "Output path from start to end without using the StartDirection")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISettings>().m_WireDirectionHintRadius = 2.f;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(9.f, 10.f);
    env.Update();

    {
        world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(15, 11);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 6);
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(11, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[2] == glm::ivec2(12, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[3] == glm::ivec2(13, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[4] == glm::ivec2(14, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[5] == glm::ivec2(15, 11));
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(10.f, 11.f);
    env.Update();

    {
        world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(7, 5);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 6);
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(9, 9));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[2] == glm::ivec2(8, 8));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[3] == glm::ivec2(7, 7));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[4] == glm::ivec2(7, 6));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[5] == glm::ivec2(7, 5));
    }
}

SYSTEM_TEST_CASE("Start and end direction is at 90 degrees to StartDirection -> "
    "Output path from start to end using the StartDirection initially")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISettings>().m_WireDirectionHintRadius = 2.f;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIPreviewWireComponent>().m_Start = glm::ivec2(10, 10);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(10.f, 11.f);
    env.Update();

    {
        world.get_mut<xg::WorldMouseComponent>().m_Cell = glm::ivec2(15, 11);
        env.Update();
        REQUIRE(preview.has<xg::UIWirePathComponent>());
        REQUIRE(preview.get<xg::UIWirePathComponent>().m_Path.size() == 7);
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[0] == glm::ivec2(10, 10));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[1] == glm::ivec2(10, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[2] == glm::ivec2(11, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[3] == glm::ivec2(12, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[4] == glm::ivec2(13, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[5] == glm::ivec2(14, 11));
        CHECK(preview.get<xg::UIWirePathComponent>().m_Path[6] == glm::ivec2(15, 11));
    }
}