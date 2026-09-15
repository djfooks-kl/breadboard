#include "BreadTest.h"
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Core/MathConstants.h"
#include "Cogs/CogMap.h"
#include "MappedInputComponent.h"
#include "OnStageComponent.h"
#include "RenderSettings.h"
#include "SelectionBoxComponent.h"
#include "SelectionBoxSystem.h"
#include "UISelectComponent.h"
#include "WireComponent.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::SelectionBoxSystem - " description, "[xg::SelectionBoxSystem]")

namespace
{
    const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");

    struct TestCog1 final : public xg::CogPrototype
    {
        xg::CogResourceId GetResourceId() const override { return s_TestCog1; }

        glm::ivec2 GetSize() const override { return glm::ivec2(3, 2); }
    };

    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::UISelectComponent>();
            m_World.ensure<xg::SelectionBoxComponent>();

            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<TestCog1>();

            auto& renderSettings = m_World.ensure<xg::RenderSettings>();
            renderSettings.m_NodeOuterRadius = 0.25f;
            renderSettings.m_CogBoxSize = 0.25f;
            renderSettings.m_WireDotOuterRadius = 0.25f;
        }

        void Update()
        {
            xg::SelectionBoxSystem::Update(m_World);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("Select a box with 1 entity in it -> Add the entity to the box")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.ensure<xg::CogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::OnStageComponent>();

    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 1);
    CHECK(entities[0] == entity);
}

SYSTEM_TEST_CASE("Has a cog but box start is not set -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.ensure<xg::CogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::OnStageComponent>();

    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 0);
}

SYSTEM_TEST_CASE("Has a cog to select but m_SelectBox=false -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = false;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.ensure<xg::CogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::OnStageComponent>();

    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 0);
}

SYSTEM_TEST_CASE("Select a box with 1 entity in it, then set m_SelectBox=false -> Entities should be empty")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.ensure<xg::CogComponent>().m_CogId = s_TestCog1;
    entity.add<xg::OnStageComponent>();
    env.Update();

    world.get_mut<xg::UISelectComponent>().m_SelectBox = false;
    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 0);
}

SYSTEM_TEST_CASE("Select a box with 1 entity in it, but entity is not on stage -> Entities should be empty")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.ensure<xg::CogComponent>().m_CogId = s_TestCog1;

    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 0);
}

SYSTEM_TEST_CASE("Select a box with 1 entity in it, but entity is not a cog/wire -> Entities should be empty")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();

    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 0);
}

SYSTEM_TEST_CASE("Select a box with 1 cog outside of it -> Entities should be empty")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& cog = entity.ensure<xg::CogComponent>();
    cog.m_CogId = s_TestCog1;

    cog.m_Transform.m_Translation = glm::ivec2(101, 50);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);

    cog.m_Transform.m_Translation = glm::ivec2(50, 101);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);
}

SYSTEM_TEST_CASE("Cog box size intersection tests")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_CogBoxSize = 0.25f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(10.f, 10.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& cog = entity.ensure<xg::CogComponent>();
    cog.m_CogId = s_TestCog1;

    cog.m_Transform.m_Translation = glm::ivec2(10, 10);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(9.76f, 9.76f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(9.76f, 9.76f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.f, 0.f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(9.74f, 9.74f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.f, 0.f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);
}

SYSTEM_TEST_CASE("Wire inside box -> Add wire to entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.25f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(10.f, 10.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);
}

SYSTEM_TEST_CASE("Wire inside box, no OnStageComponent -> Do not add wire to entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.25f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(10.f, 10.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

    flecs::entity entity = world.entity();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);
}

SYSTEM_TEST_CASE("Has a wire but box start is not set -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));
    entity.add<xg::OnStageComponent>();

    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 0);
}

SYSTEM_TEST_CASE("Has a wire to select but m_SelectBox=false -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = false;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));
    entity.add<xg::OnStageComponent>();

    env.Update();
    const auto& entities = world.get<xg::SelectionBoxComponent>().m_Entities;
    REQUIRE(entities.size() == 0);
}

SYSTEM_TEST_CASE("Select a box with 1 wire outside of it -> Entities should be empty")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.25f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(100.f, 100.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(101, 15));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);

    wire.m_Checkpoints[0] = glm::ivec2(15, 101);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);
}

SYSTEM_TEST_CASE("Wire dot intersection tests")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.25f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(10.f, 10.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(10, 10));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    const float diagonal = 0.25f * ONE_OVER_SQRT_2;
    const float justInside = (10.f - diagonal) + 0.01f;
    const float justOutside = (10.f - diagonal) - 0.01f;

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(justInside, justInside);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(justInside, justInside);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.f, 0.f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(justOutside, justOutside);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.f, 0.f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);
}

SYSTEM_TEST_CASE("Wire segment intersection test")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.f;
    renderSettings.m_WireOuterWidth = ONE_OVER_SQRT_2 * 0.5f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(10.f, 10.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(10, 10));
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(10.f, 15.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(12.f, 12.f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(15.f, 10.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(12.f, 11.49f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 0);

    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(10.f, 15.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(12.f, 11.51f);
    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);
}

SYSTEM_TEST_CASE("Wire multiple segments intersection test")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.f;
    renderSettings.m_WireOuterWidth = ONE_OVER_SQRT_2 * 0.5f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(12.f, 11.51f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(10.f, 15.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(10, 5));
    wire.m_Checkpoints.push_back(glm::ivec2(10, 10));
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);
}

SYSTEM_TEST_CASE("Wire with 0 checkpoints -> Don't crash")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.f;
    renderSettings.m_WireOuterWidth = ONE_OVER_SQRT_2 * 0.5f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(12.f, 11.51f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(10.f, 16.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    entity.ensure<xg::WireComponent>();

    env.Update();
}

SYSTEM_TEST_CASE("Wire multiple segments inside intersection test")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.f;
    renderSettings.m_WireOuterWidth = ONE_OVER_SQRT_2 * 0.5f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(10, 5));
    wire.m_Checkpoints.push_back(glm::ivec2(10, 10));
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);
}

SYSTEM_TEST_CASE("Wire multiple segments and dots inside intersection test")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.25f;
    renderSettings.m_WireOuterWidth = ONE_OVER_SQRT_2 * 0.5f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(10, 5));
    wire.m_Checkpoints.push_back(glm::ivec2(10, 10));
    wire.m_Checkpoints.push_back(glm::ivec2(15, 15));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);
}

SYSTEM_TEST_CASE("Wire multiple dots intersection test")
{
    TestEnv env;
    flecs::world world = env.m_World;

    auto& renderSettings = world.ensure<xg::RenderSettings>();
    renderSettings.m_WireDotOuterRadius = 0.25f;
    renderSettings.m_WireOuterWidth = 0.f;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    world.get_mut<xg::UISelectComponent>().m_BoxStart = glm::vec2(0.f, 0.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(10.f, 10.76f);

    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    auto& wire = entity.ensure<xg::WireComponent>();
    wire.m_Checkpoints.push_back(glm::ivec2(10, 15));
    wire.m_Checkpoints.push_back(glm::ivec2(10, 11));

    env.Update();
    CHECK(world.get<xg::SelectionBoxComponent>().m_Entities.size() == 1);
}
