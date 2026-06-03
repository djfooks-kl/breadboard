#include "BreadTest.h"
#include <flecs/flecs.h>

#include "UIWireSegmentsSystem.h"
#include "UIWirePathComponent.h"
#include "UIWireSegmentsComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIWireSegmentsSystem - " description, "[xg::UIWireSegmentsSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
        }

        void Update()
        {
            xg::UIWireSegmentsSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("No cells -> No segments")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIWirePathComponent>();

    env.Update();

    REQUIRE(preview.has<xg::UIWireSegmentsComponent>());
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints.size() == 0);
}

SYSTEM_TEST_CASE("Single cell -> Single segment with start=end")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    preview.ensure<xg::UIWirePathComponent>().m_Path.push_back(glm::ivec2(1, 2));

    env.Update();

    REQUIRE(preview.has<xg::UIWireSegmentsComponent>());
    REQUIRE(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints.size() == 1);
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[0] == glm::ivec2(1, 2));
}

SYSTEM_TEST_CASE("2 cells -> Single segment")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    auto& path = preview.ensure<xg::UIWirePathComponent>().m_Path;
    path.push_back(glm::ivec2(1, 2));
    path.push_back(glm::ivec2(1, 3));

    env.Update();

    REQUIRE(preview.has<xg::UIWireSegmentsComponent>());
    REQUIRE(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints.size() == 2);
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[0] == glm::ivec2(1, 2));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[1] == glm::ivec2(1, 3));
}

SYSTEM_TEST_CASE("3 cells with a bend -> 2 segments")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    auto& path = preview.ensure<xg::UIWirePathComponent>().m_Path;
    path.push_back(glm::ivec2(1, 2));
    path.push_back(glm::ivec2(1, 3));
    path.push_back(glm::ivec2(2, 3));

    env.Update();

    REQUIRE(preview.has<xg::UIWireSegmentsComponent>());
    REQUIRE(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints.size() == 3);
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[0] == glm::ivec2(1, 2));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[1] == glm::ivec2(1, 3));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[2] == glm::ivec2(2, 3));
}

SYSTEM_TEST_CASE("3 cells in a straight line -> 2 segments")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    auto& path = preview.ensure<xg::UIWirePathComponent>().m_Path;
    path.push_back(glm::ivec2(1, 2));
    path.push_back(glm::ivec2(1, 3));
    path.push_back(glm::ivec2(1, 4));

    env.Update();

    REQUIRE(preview.has<xg::UIWireSegmentsComponent>());
    REQUIRE(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints.size() == 2);
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[0] == glm::ivec2(1, 2));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[1] == glm::ivec2(1, 4));
}

SYSTEM_TEST_CASE("Path changes -> Update segments")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    {
        auto& path = preview.ensure<xg::UIWirePathComponent>().m_Path;
        path.push_back(glm::ivec2(1, 2));
        path.push_back(glm::ivec2(1, 3));
        path.push_back(glm::ivec2(2, 3));
    }
    env.Update();

    {
        auto& path = preview.get_mut<xg::UIWirePathComponent>().m_Path;
        path.clear();
        path.push_back(glm::ivec2(3, 2));
        path.push_back(glm::ivec2(3, 1));
    }
    env.Update();

    REQUIRE(preview.has<xg::UIWireSegmentsComponent>());
    REQUIRE(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints.size() == 2);
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[0] == glm::ivec2(3, 2));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[1] == glm::ivec2(3, 1));
}

SYSTEM_TEST_CASE("Mix of straight lines and bends")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity preview = world.entity();
    auto& path = preview.ensure<xg::UIWirePathComponent>().m_Path;
    path.push_back(glm::ivec2(1, 2));
    path.push_back(glm::ivec2(1, 3));
    path.push_back(glm::ivec2(1, 4));
    path.push_back(glm::ivec2(2, 5));
    path.push_back(glm::ivec2(3, 6));
    path.push_back(glm::ivec2(4, 7));
    path.push_back(glm::ivec2(3, 7));
    path.push_back(glm::ivec2(2, 7));
    path.push_back(glm::ivec2(1, 7));
    path.push_back(glm::ivec2(0, 7));
    path.push_back(glm::ivec2(0, 6));
    path.push_back(glm::ivec2(0, 5));
    path.push_back(glm::ivec2(0, 4));

    env.Update();

    REQUIRE(preview.has<xg::UIWireSegmentsComponent>());
    REQUIRE(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints.size() == 5);
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[0] == glm::ivec2(1, 2));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[1] == glm::ivec2(1, 4));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[2] == glm::ivec2(4, 7));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[3] == glm::ivec2(0, 7));
    CHECK(preview.get<xg::UIWireSegmentsComponent>().m_Checkpoints[4] == glm::ivec2(0, 4));
}