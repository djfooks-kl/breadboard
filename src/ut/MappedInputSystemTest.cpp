#include "BreadTest.h"
#include <flecs/flecs.h>

#include "InputComponent.h"
#include "KeybindingSettings.h"
#include "MappedInputComponent.h"
#include "MappedInputSystem.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::MappedInputSystem - " description, "[xg::MappedInputSystem]")

namespace
{
    struct TestEnv
    {
        TestEnv()
        {
            m_World.ensure<xg::InputComponent>();
            m_World.ensure<xg::MappedInputComponent>();
            m_World.ensure<xg::KeybindingSettings>();
        }

        void Update()
        {
            xg::MappedInputSystem::Update(m_World);
            m_World.get_mut<xg::InputComponent>().m_KeyDown.clear();
            m_World.get_mut<xg::InputComponent>().m_KeyPress.clear();
            m_World.get_mut<xg::InputComponent>().m_KeyRelease.clear();
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("m_KeyDown: Input has bound keys -> Remap to bindings")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyDown.insert('w');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.size() == 1);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::MoveUp));
}

SYSTEM_TEST_CASE("m_KeyDown: No bindings -> Don't set any inputs")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyDown.insert('w');

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.empty());
}

SYSTEM_TEST_CASE("m_KeyDown: Unbound keys -> Don't set any inputs")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyDown.insert('w');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['x'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.empty());
}

SYSTEM_TEST_CASE("m_KeyDown: Bound and unbound keys -> Set inputs for the bound keys")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyDown.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyDown.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.size() == 1);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::MoveUp));
}

SYSTEM_TEST_CASE("m_KeyDown: Multiple bound keys -> Set inputs for the bound keys")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyDown.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyDown.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;
    world.get_mut<xg::KeybindingSettings>().m_Bindings['s'] = xg::EMappedInput::MoveDown;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.size() == 2);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::MoveUp));
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::MoveDown));
}

SYSTEM_TEST_CASE("m_KeyDown: Multiple bound keys, then next frame none -> Clear the set")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyDown.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyDown.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;
    world.get_mut<xg::KeybindingSettings>().m_Bindings['s'] = xg::EMappedInput::MoveDown;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.size() == 2);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::MoveUp));
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::MoveDown));

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyDown.empty());
}

SYSTEM_TEST_CASE("m_KeyPress: Input has bound keys -> Remap to bindings")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyPress.insert('w');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.size() == 1);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.contains(xg::EMappedInput::MoveUp));
}

SYSTEM_TEST_CASE("m_KeyPress: No bindings -> Don't set any inputs")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyPress.insert('w');

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.empty());
}

SYSTEM_TEST_CASE("m_KeyPress: Unbound keys -> Don't set any inputs")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyPress.insert('w');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['x'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.empty());
}

SYSTEM_TEST_CASE("m_KeyPress: Bound and unbound keys -> Set inputs for the bound keys")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyPress.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyPress.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.size() == 1);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.contains(xg::EMappedInput::MoveUp));
}

SYSTEM_TEST_CASE("m_KeyPress: Multiple bound keys -> Set inputs for the bound keys")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyPress.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyPress.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;
    world.get_mut<xg::KeybindingSettings>().m_Bindings['s'] = xg::EMappedInput::MoveDown;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.size() == 2);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.contains(xg::EMappedInput::MoveUp));
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.contains(xg::EMappedInput::MoveDown));
}

SYSTEM_TEST_CASE("m_KeyPress: Multiple bound keys, then next frame none -> Clear the set")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyPress.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyPress.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;
    world.get_mut<xg::KeybindingSettings>().m_Bindings['s'] = xg::EMappedInput::MoveDown;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.size() == 2);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.contains(xg::EMappedInput::MoveUp));
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.contains(xg::EMappedInput::MoveDown));

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyPress.empty());
}

SYSTEM_TEST_CASE("m_KeyRelease: Input has bound keys -> Remap to bindings")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('w');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.size() == 1);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.contains(xg::EMappedInput::MoveUp));
}

SYSTEM_TEST_CASE("m_KeyRelease: No bindings -> Don't set any inputs")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('w');

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.empty());
}

SYSTEM_TEST_CASE("m_KeyRelease: Unbound keys -> Don't set any inputs")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('w');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['x'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.empty());
}

SYSTEM_TEST_CASE("m_KeyRelease: Bound and unbound keys -> Set inputs for the bound keys")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.size() == 1);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.contains(xg::EMappedInput::MoveUp));
}

SYSTEM_TEST_CASE("m_KeyRelease: Multiple bound keys -> Set inputs for the bound keys")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;
    world.get_mut<xg::KeybindingSettings>().m_Bindings['s'] = xg::EMappedInput::MoveDown;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.size() == 2);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.contains(xg::EMappedInput::MoveUp));
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.contains(xg::EMappedInput::MoveDown));
}

SYSTEM_TEST_CASE("m_KeyRelease: Multiple bound keys, then next frame none -> Clear the set")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('w');
    world.get_mut<xg::InputComponent>().m_KeyRelease.insert('s');

    world.get_mut<xg::KeybindingSettings>().m_Bindings['w'] = xg::EMappedInput::MoveUp;
    world.get_mut<xg::KeybindingSettings>().m_Bindings['s'] = xg::EMappedInput::MoveDown;

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.size() == 2);
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.contains(xg::EMappedInput::MoveUp));
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.contains(xg::EMappedInput::MoveDown));

    env.Update();
    CHECK(world.get<xg::MappedInputComponent>().m_KeyRelease.empty());
}