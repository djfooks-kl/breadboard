#include "BreadTest.h"
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Core/GLFWLib.h"
#include "MappedInputComponent.h"
#include "RenderSettings.h"
#include "SelectedComponent.h"
#include "SelectionChangedComponent.h"
#include "SelectionSystem.h"
#include "UISelectComponent.h"
#include "WireComponent.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::SelectionSystem - " description, "[xg::SelectionSystem]")

namespace
{
    const xg::CogResourceId s_OneCellCog = xg::CogResourceId::Create("OneCellCog");
    const xg::CogResourceId s_TestCog1 = xg::CogResourceId::Create("TestCog1");

    struct OneCellCog final : public xg::CogPrototype
    {
        xg::CogResourceId GetResourceId() const override { return s_OneCellCog; }

        glm::ivec2 GetSize() const override { return glm::ivec2(1, 1); }
    };

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
            m_World.ensure<xg::MappedInputComponent>();
            m_World.ensure<xg::UISelectComponent>();

            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<OneCellCog>();
            cogMap.Register<TestCog1>();

            auto& renderSettings = m_World.ensure<xg::RenderSettings>();
            renderSettings.m_NodeOuterRadius = 0.25f;
            renderSettings.m_CogBoxSize = 0.25f;
            renderSettings.m_WireDotOuterRadius = 0.25f;
        }

        void Update()
        {
            xg::SelectionSystem::Update(m_World);
            m_World.get_mut<xg::UISelectComponent>().m_SelectEntity = flecs::entity::null();
            m_World.get_mut<xg::UISelectComponent>().m_Clear = false;
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("UISelectComponent has entity set -> Add a SelectedComponent to entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entity;

    env.Update();
    CHECK(entity.has<xg::SelectedComponent>());
    CHECK(world.has<xg::SelectionChangedComponent>());
}

SYSTEM_TEST_CASE("UISelectComponent has no entity set -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectEntity = flecs::entity::null();

    env.Update();
    CHECK(world.count<xg::SelectedComponent>() == 0);
    CHECK(world.has<xg::SelectionChangedComponent>() == false);
}

SYSTEM_TEST_CASE("UISelectComponent has entity set, next frame no entity set -> Remove SelectionChangedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entity;

    env.Update();

    env.Update();
    CHECK(world.has<xg::SelectionChangedComponent>() == false);
}

SYSTEM_TEST_CASE("UISelectComponent has entity set, next frame same entity set -> "
    "SelectionComponent should stay, remove SelectionChangedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entity;

    env.Update();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entity;

    env.Update();
    CHECK(entity.has<xg::SelectedComponent>());
    CHECK(world.has<xg::SelectionChangedComponent>() == false);
}

SYSTEM_TEST_CASE("Select entityA, then select entityB -> Clear SelectedComponent on entityA, add it to entityB")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    flecs::entity entityB = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;

    env.Update();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityB;

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == false);
    CHECK(entityB.has<xg::SelectedComponent>() == true);
    CHECK(world.has<xg::SelectionChangedComponent>() == true);
}

SYSTEM_TEST_CASE("Select entityA, then select entityB, while holding RIGHT_SHIFT -> "
    "Keep SelectedComponent on entityA, add it to entityB")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    flecs::entity entityB = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;

    env.Update();
    world.get_mut<xg::MappedInputComponent>().m_KeyDown.insert(xg::EMappedInput::Multiselect);
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityB;

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == true);
    CHECK(entityB.has<xg::SelectedComponent>() == true);
    CHECK(world.has<xg::SelectionChangedComponent>() == true);
}

SYSTEM_TEST_CASE("RIGHT_SHIFT selection toggles selection -> "
    "Keep entityB selected, remove SelectedComponent on entityA")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    flecs::entity entityB = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;

    env.Update();
    world.get_mut<xg::MappedInputComponent>().m_KeyDown.insert(xg::EMappedInput::Multiselect);
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityB;

    env.Update();
    world.get_mut<xg::MappedInputComponent>().m_KeyDown.insert(xg::EMappedInput::Multiselect);
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == false);
    CHECK(entityB.has<xg::SelectedComponent>() == true);
    CHECK(world.has<xg::SelectionChangedComponent>() == true);
}

SYSTEM_TEST_CASE("Select multiple entities, then set Clear=true -> "
    "Remove SelectedComponent from all entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    flecs::entity entityB = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;

    env.Update();
    world.get_mut<xg::MappedInputComponent>().m_KeyDown.insert(xg::EMappedInput::Multiselect);
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityB;

    env.Update();
    world.get_mut<xg::UISelectComponent>().m_Clear = true;

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == false);
    CHECK(entityB.has<xg::SelectedComponent>() == false);
    CHECK(world.has<xg::SelectionChangedComponent>() == true);
}

SYSTEM_TEST_CASE("Select nothing, then set Clear=true -> "
    "Do not add the SelectionChangedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_Clear = true;

    env.Update();
    CHECK(world.has<xg::SelectionChangedComponent>() == false);
}

SYSTEM_TEST_CASE("Select something and set Clear=true same frame -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;
    world.get_mut<xg::UISelectComponent>().m_Clear = true;

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == false);
    CHECK(world.has<xg::SelectionChangedComponent>() == false);
}
