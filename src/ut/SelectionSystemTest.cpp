#include "BreadTest.h"
#include <flecs/flecs.h>

#include "Core/GLFWLib.h"
#include "MappedInputComponent.h"
#include "OnStageRemovedComponent.h"
#include "SelectedComponent.h"
#include "SelectionBoxComponent.h"
#include "SelectionChangedComponent.h"
#include "SelectionSystem.h"
#include "UISelectComponent.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::SelectionSystem - " description, "[xg::SelectionSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::MappedInputComponent>();
            m_World.ensure<xg::UISelectComponent>();
            m_World.ensure<xg::SelectionBoxComponent>();
        }

        void Update()
        {
            xg::SelectionSystem::Update(m_World);
            m_World.get_mut<xg::UISelectComponent>().m_SelectEntity = flecs::entity::null();
            m_World.get_mut<xg::UISelectComponent>().m_SelectBox = false;

            m_World.get_mut<xg::SelectionBoxComponent>().m_Entities.clear();
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

SYSTEM_TEST_CASE("Select entityA, then select entityB, while holding Multiselect -> "
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

SYSTEM_TEST_CASE("Multiselect selection toggles selection -> "
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

SYSTEM_TEST_CASE("Select multiple entities, then set m_SelectBox=true with an empty box -> "
    "Remove SelectedComponent from all entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    flecs::entity entityB = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;

    env.Update();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityB;

    env.Update();
    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == false);
    CHECK(entityB.has<xg::SelectedComponent>() == false);
    CHECK(world.has<xg::SelectionChangedComponent>() == true);
}

SYSTEM_TEST_CASE("Select nothing, then set m_SelectBox=true with an empty box -> "
    "Do not add the SelectionChangedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;

    env.Update();
    CHECK(world.has<xg::SelectionChangedComponent>() == false);
}

SYSTEM_TEST_CASE("Select nothing, then set m_SelectBox=true with multile entities in box -> "
    "Select the entities, add the SelectionChangedComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;

    flecs::entity entity1 = world.entity();
    flecs::entity entity2 = world.entity();
    world.get_mut<xg::SelectionBoxComponent>().m_Entities.push_back(entity1);
    world.get_mut<xg::SelectionBoxComponent>().m_Entities.push_back(entity2);

    env.Update();
    CHECK(world.has<xg::SelectionChangedComponent>() == true);
    CHECK(entity1.has<xg::SelectedComponent>());
    CHECK(entity2.has<xg::SelectedComponent>());
}

SYSTEM_TEST_CASE("Entity already selected, while holding Multiselect set m_SelectBox=true with multile entities in box -> Add a SelectedComponent to new entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity1 = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entity1;
    env.Update();

    world.get_mut<xg::MappedInputComponent>().m_KeyDown.insert(xg::EMappedInput::Multiselect);
    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;
    flecs::entity entity2 = world.entity();
    flecs::entity entity3 = world.entity();
    world.get_mut<xg::SelectionBoxComponent>().m_Entities.push_back(entity2);
    world.get_mut<xg::SelectionBoxComponent>().m_Entities.push_back(entity3);

    env.Update();
    CHECK(entity1.has<xg::SelectedComponent>());
    CHECK(entity2.has<xg::SelectedComponent>());
    CHECK(entity3.has<xg::SelectedComponent>());
    CHECK(world.has<xg::SelectionChangedComponent>());
}

SYSTEM_TEST_CASE("Select something and set m_SelectBox=true with empty box same frame -> Do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;
    world.get_mut<xg::UISelectComponent>().m_SelectBox = true;

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == false);
    CHECK(world.has<xg::SelectionChangedComponent>() == false);
}

SYSTEM_TEST_CASE("Select something then remove it from stage -> Remove it from selection")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entityA = world.entity();
    world.get_mut<xg::UISelectComponent>().m_SelectEntity = entityA;

    env.Update();
    entityA.add<xg::OnStageRemovedComponent>();

    env.Update();
    CHECK(entityA.has<xg::SelectedComponent>() == false);
    CHECK(world.has<xg::SelectionChangedComponent>() == true);
}
