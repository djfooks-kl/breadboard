#include "BreadTest.h"
#include <flecs/flecs.h>

#include "HoverVFXComponent.h"
#include "HoverVFXSystem.h"
#include "UIHoverComponent.h"
#include "UISettings.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::HoverVFXSystem - " description, "[xg::HoverVFXSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::UIHoverComponent>();
            m_World.ensure<xg::UISettings>().m_HoverDuration = 1000.f;
        }

        void Update(float deltaTime = 0.f)
        {
            xg::HoverVFXSystem::Update(m_World, deltaTime);
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("When hovering an entity -> Add a HoverVFXComponent, copy the start mouse position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.5f, 2.f);
    env.Update(0.5f);
    REQUIRE(entity.has<xg::HoverVFXComponent>());
    CHECK(entity.get<xg::HoverVFXComponent>().m_WasHovering == true);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InDuration == 0.f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InPosition == glm::vec2(1.5f, 2.f));
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutDuration == 0.f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutPosition == glm::vec2(0.f, 0.f));
}

SYSTEM_TEST_CASE("When hovering an entity, next frame -> Keep the HoverVFXComponent, update the duration, do not update the m_InPosition")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.5f, 2.f);
    env.Update();

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.f, 2.5f);
    env.Update(0.5f);
    REQUIRE(entity.has<xg::HoverVFXComponent>());
    CHECK(entity.get<xg::HoverVFXComponent>().m_InDuration == 0.5f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InPosition == glm::vec2(1.5f, 2.f));
}

SYSTEM_TEST_CASE("When hovering an entity, next 2 frames -> Keep the HoverVFXComponent, increase the duration by deltaTime")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    env.Update();

    env.Update(0.75f);
    env.Update(0.75f);
    REQUIRE(entity.has<xg::HoverVFXComponent>());
    CHECK(entity.get<xg::HoverVFXComponent>().m_InDuration == 1.5f);
}

SYSTEM_TEST_CASE("When hovering nothing -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = flecs::entity::null();
    env.Update(0.25f);
}

SYSTEM_TEST_CASE("When hovering an entity, then next frame hovering nothing -> Set m_WasHovering=false, don't increase the m_InDuration, copy mouse to m_OutPosition")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.5f, 1.f);
    env.Update();
    env.Update(0.25f);

    world.get_mut<xg::UIHoverComponent>().m_Entity = flecs::entity::null();
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.5f, 2.f);
    env.Update(0.5f);

    REQUIRE(entity.has<xg::HoverVFXComponent>() == true);
    CHECK(entity.get<xg::HoverVFXComponent>().m_WasHovering == false);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InDuration == 0.25f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InPosition == glm::vec2(0.5f, 1.f));
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutDuration == 0.f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutPosition == glm::vec2(1.5f, 2.f));
}

SYSTEM_TEST_CASE("When hovering an entity, then next frame hovering a different entity -> Update both entities simulatanously")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity1 = world.entity();
    flecs::entity entity2 = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity1;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.5f, 1.f);
    env.Update();
    env.Update(0.25f);

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity2;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.5f, 2.f);
    env.Update(0.5f);

    REQUIRE(entity1.has<xg::HoverVFXComponent>());
    CHECK(entity1.get<xg::HoverVFXComponent>().m_WasHovering == false);
    CHECK(entity1.get<xg::HoverVFXComponent>().m_InDuration == 0.25f);
    CHECK(entity1.get<xg::HoverVFXComponent>().m_InPosition == glm::vec2(0.5f, 1.f));
    CHECK(entity1.get<xg::HoverVFXComponent>().m_OutDuration == 0.f);
    CHECK(entity1.get<xg::HoverVFXComponent>().m_OutPosition == glm::vec2(1.5f, 2.f));

    REQUIRE(entity2.has<xg::HoverVFXComponent>());
    CHECK(entity2.get<xg::HoverVFXComponent>().m_WasHovering == true);
    CHECK(entity2.get<xg::HoverVFXComponent>().m_InDuration == 0.f);
    CHECK(entity2.get<xg::HoverVFXComponent>().m_InPosition == glm::vec2(1.5f, 2.f));
}

SYSTEM_TEST_CASE("After hovering an entity -> increase the m_OutDuration, don't copy mouse to m_OutPosition")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.5f, 1.f);
    env.Update();
    env.Update(0.25f);

    world.get_mut<xg::UIHoverComponent>().m_Entity = flecs::entity::null();
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.5f, 2.f);
    env.Update(0.5f);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.5f, 3.f);
    env.Update(0.5f);

    REQUIRE(entity.has<xg::HoverVFXComponent>() == true);
    CHECK(entity.get<xg::HoverVFXComponent>().m_WasHovering == false);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InDuration == 0.25f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InPosition == glm::vec2(0.5f, 1.f));
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutDuration == 0.5f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutPosition == glm::vec2(1.5f, 2.f));
}

SYSTEM_TEST_CASE("After hovering an entity, next frame -> keep increasing the m_OutDuration")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    env.Update();
    env.Update(0.25f);

    world.get_mut<xg::UIHoverComponent>().m_Entity = flecs::entity::null();
    env.Update(0.5f);

    env.Update(0.5f);
    env.Update(0.75f);

    REQUIRE(entity.has<xg::HoverVFXComponent>() == true);
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutDuration == 1.25f);
}

SYSTEM_TEST_CASE("After hovering an entity, the OutDuration>HoverDuration -> Remove the HoverVFXComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;
    world.get_mut<xg::UISettings>().m_HoverDuration = 3.f;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    env.Update();
    env.Update(0.25f);

    world.get_mut<xg::UIHoverComponent>().m_Entity = flecs::entity::null();
    env.Update(0.5f);

    env.Update(2.99f);
    CHECK(entity.has<xg::HoverVFXComponent>() == true);
    env.Update(0.02f);
    CHECK(entity.has<xg::HoverVFXComponent>() == false);
}

SYSTEM_TEST_CASE("After hovering an entity, hovering starts again -> "
    "Set m_WasHovering=true, set m_InDuration and m_InPosition, reset m_OutDuration and m_OutPosition")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.5f, 1.f);
    env.Update();
    env.Update(0.25f);

    world.get_mut<xg::UIHoverComponent>().m_Entity = flecs::entity::null();
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.5f, 2.f);
    env.Update(0.5f);
    env.Update(0.5f);

    world.get_mut<xg::UIHoverComponent>().m_Entity = entity;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.5f, 3.f);
    env.Update(0.5f);

    REQUIRE(entity.has<xg::HoverVFXComponent>() == true);
    CHECK(entity.get<xg::HoverVFXComponent>().m_WasHovering == true);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InDuration == 0.f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_InPosition == glm::vec2(2.5f, 3.f));
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutDuration == 0.f);
    CHECK(entity.get<xg::HoverVFXComponent>().m_OutPosition == glm::vec2(0.f, 0.f));
}

// todo re hover before removal