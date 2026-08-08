#include "BreadTest.h"
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Core/Range.h"
#include "GridAttachmentsComponent.h"
#include "RenderSettings.h"
#include "UIHoverComponent.h"
#include "UIHoverSystem.h"
#include "UISettings.h"
#include "WireComponent.h"
#include "WireHelpers.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIHoverSystem - " description, "[xg::UIHoverSystem]")

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
            m_World.ensure<xg::GridAttachmentsComponent>();
            m_World.ensure<xg::WorldMouseComponent>();
            m_World.ensure<xg::UIHoverComponent>();

            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<OneCellCog>();
            cogMap.Register<TestCog1>();

            auto& renderSettings = m_World.ensure<xg::RenderSettings>();
            renderSettings.m_NodeOuterRadius = 0.25f;
            renderSettings.m_CogBoxSize = 0.25f;
            renderSettings.m_WireDotOuterRadius = 0.25f;

            auto& uiSettings = m_World.ensure<xg::UISettings>();
            uiSettings.m_WireHoverWidth = 0.25f;
        }

        void Update()
        {
            xg::UIHoverSystem::Update(m_World);
        }

        flecs::world m_World;
    };

    bool IsHoveringWireSegment(TestEnv& env, xg::TWireDirectionFlags flags, const glm::vec2& mouse, const float width=0.25f)
    {
        flecs::world world = env.m_World;
        flecs::entity wire = world.entity();
        {
            std::vector<glm::ivec2>& checkpoints = wire.ensure<xg::WireComponent>().m_Checkpoints;
            checkpoints.push_back(glm::ivec2(0, 0));

            if (flags.HasAll(xg::EWireDirection::All))
            {
                checkpoints.push_back(glm::ivec2(0, 1));
                checkpoints.push_back(glm::ivec2(0, 0));
                checkpoints.push_back(glm::ivec2(1, 1));
                checkpoints.push_back(glm::ivec2(0, 0));
                checkpoints.push_back(glm::ivec2(1, 0));
                checkpoints.push_back(glm::ivec2(0, 0));
                checkpoints.push_back(glm::ivec2(1, -1));
                checkpoints.push_back(glm::ivec2(0, 0));
                checkpoints.push_back(glm::ivec2(0, -1));
                checkpoints.push_back(glm::ivec2(0, 0));
                checkpoints.push_back(glm::ivec2(-1, -1));
                checkpoints.push_back(glm::ivec2(0, 0));
                checkpoints.push_back(glm::ivec2(-1, 0));
                checkpoints.push_back(glm::ivec2(0, 0));
                checkpoints.push_back(glm::ivec2(-1, 1));
                checkpoints.push_back(glm::ivec2(0, 0));
            }
            else
            {
                switch (static_cast<xg::EWireDirection>(flags.GetValue()))
                {
                case xg::EWireDirection::N:  checkpoints.push_back(glm::ivec2(0, 1)); break;
                case xg::EWireDirection::NE: checkpoints.push_back(glm::ivec2(1, 1)); break;
                case xg::EWireDirection::E:  checkpoints.push_back(glm::ivec2(1, 0)); break;
                case xg::EWireDirection::SE: checkpoints.push_back(glm::ivec2(1, -1)); break;
                case xg::EWireDirection::S:  checkpoints.push_back(glm::ivec2(0, -1)); break;
                case xg::EWireDirection::SW: checkpoints.push_back(glm::ivec2(-1, -1)); break;
                case xg::EWireDirection::W:  checkpoints.push_back(glm::ivec2(-1, 0)); break;
                case xg::EWireDirection::NW: checkpoints.push_back(glm::ivec2(-1, 1)); break;
                }
            }

            auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
            gridAttachmentsComponent.m_Map[glm::ivec2(0, 0)].m_WireDirectionFlags = flags;
            for (const glm::ivec2& checkpoint : checkpoints)
            {
                std::vector<flecs::entity>& entities = gridAttachmentsComponent.m_Map[checkpoint].m_Entities;
                if (!xc::RangeContains(entities, wire))
                {
                    gridAttachmentsComponent.m_Map[checkpoint].m_Entities.push_back(wire);
                }
            }
            world.get_mut<xg::UISettings>().m_WireHoverWidth = width;
            world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.f;
        }

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(mouse);
        env.Update();
        return world.get<xg::UIHoverComponent>().m_Wire == wire;
    }
}

SYSTEM_TEST_CASE("While hovering a cog node -> set the node flag to true")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasNode = true;

        world.get_mut<xg::RenderSettings>().m_NodeOuterRadius = 100.f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Node);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.51f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Node);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.49f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Node);
}

SYSTEM_TEST_CASE("While not hovering a cog node cell -> set the node flag to false")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasNode = true;

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.f, 2.f);
    }

    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Node == false);
}

SYSTEM_TEST_CASE("While hovering a cog node cell but not the cog node circle -> set the node flag to false")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasNode = true;

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.26f, 2.f);
    }

    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Node == false);
}

SYSTEM_TEST_CASE("While hovering a wire checkpoint -> set the wire entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasWireCheckpoint = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(world.entity());
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 100.f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.51f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.49f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);
}

SYSTEM_TEST_CASE("While hovering a wire dot on a cog node -> set the wire entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasNode = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::E);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 100.f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.51f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.49f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);
}

SYSTEM_TEST_CASE("While not hovering a wire checkpoint cell -> set the wire to invalid")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasWireCheckpoint = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 100.f;
    }

    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Wire == flecs::entity::null());
}

SYSTEM_TEST_CASE("Hover a wire checkpoint cell then stop -> reset the wire flag to false")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasWireCheckpoint = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 100.f;
    }

    env.Update();

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(20.f, 20.f);
    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Wire == flecs::entity::null());
}

SYSTEM_TEST_CASE("While hovering a wire checkpoint cell but not the wire checkpoint circle -> set the wire to null")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasWireCheckpoint = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.25f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.26f, 2.f);
    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Wire == flecs::entity::null());
}

SYSTEM_TEST_CASE("Wire with multiple checkpoints, hovering nearby to the checkpoint -> set the wire to null")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 3));
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasWireCheckpoint = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags = xg::EWireDirection::N;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.25f;
        world.get_mut<xg::UISettings>().m_WireHoverWidth = 0.1f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.26f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == flecs::entity::null());

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.24f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);
}

SYSTEM_TEST_CASE("While hovering a wire segment line cell but not the line itself -> set the wire to null")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::E);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.4f);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.f;
        world.get_mut<xg::UISettings>().m_WireHoverWidth = 0.1f;
    }

    env.Update();

    CHECK(world.get<xg::UIHoverComponent>().m_Wire == flecs::entity::null());
}

SYSTEM_TEST_CASE("While hovering a wire segment line -> set the wire flag to true")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire = world.entity();
    {
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
        wire.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(2, 2));
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::E);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.f;
        world.get_mut<xg::UISettings>().m_WireHoverWidth = 0.1f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.49f, 2.09f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.49f, 1.91f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire);
}

SYSTEM_TEST_CASE("Check hitbox for each wire direction")
{
    TestEnv env;
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(-0.24f,  0.f    )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(0.24f,   0.f    )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(-0.24f,  0.49f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(0.24f,   0.49f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(0.f,     0.01f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(0.f,     -0.01f )) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(-0.26f,  0.49f  )) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::N, glm::vec2(0.26f,   0.49f  )) == false);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::E, glm::vec2(0.49f,   -0.24f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::E, glm::vec2(0.49f,   0.24f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::E, glm::vec2(0.01f,   0.f    )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::E, glm::vec2(-0.01f,  0.f    )) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::E, glm::vec2(0.49f,   -0.26f)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::E, glm::vec2(0.49f,   0.26f  )) == false);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::S, glm::vec2(-0.24f,  -0.49f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::S, glm::vec2(0.24f,   -0.49f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::S, glm::vec2(0.f,     -0.01f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::S, glm::vec2(0.f,     0.01f )) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::S, glm::vec2(-0.26f,  -0.49f  )) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::S, glm::vec2(0.26f,   -0.49f  )) == false);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::W, glm::vec2(-0.49f,   -0.24f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::W, glm::vec2(-0.49f,   0.24f  )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::W, glm::vec2(-0.01f,   0.f    )) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::W, glm::vec2(0.01f,    0.f    )) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::W, glm::vec2(-0.49f,   -0.26f)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::W, glm::vec2(-0.49f,   0.26f  )) == false);

    const float insideWidthDiagonal = 0.2499f * std::sqrt(2.f);
    const float outsideWidthDiagonal = 0.2501f * std::sqrt(2.f);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(0.49f, 0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(-0.01f, -0.01f)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(0.49f, 0.49f - insideWidthDiagonal)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(0.49f, 0.49f - outsideWidthDiagonal)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(0.49f - insideWidthDiagonal, 0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(0.49f - outsideWidthDiagonal, 0.49f)) == false);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(0.49f,                        -0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(-0.01f,                       0.01f)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(0.49f,                        -0.49f + insideWidthDiagonal)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(0.49f,                        -0.49f + outsideWidthDiagonal)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(0.49f - insideWidthDiagonal,  -0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(0.49f - outsideWidthDiagonal, -0.49f)) == false);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(-0.49f,                        -0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(0.01f,                         0.01f)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(-0.49f,                        -0.49f + insideWidthDiagonal)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(-0.49f,                        -0.49f + outsideWidthDiagonal)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(-0.49f + insideWidthDiagonal,  -0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(-0.49f + outsideWidthDiagonal, -0.49f)) == false);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(-0.49f,                        0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(0.01f,                         -0.01f)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(-0.49f,                        0.49f - insideWidthDiagonal)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(-0.49f,                        0.49f - outsideWidthDiagonal)) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(-0.49f + insideWidthDiagonal,  0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(-0.49f + outsideWidthDiagonal, 0.49f)) == false);
}

SYSTEM_TEST_CASE("Check hitbox for all wire directions at once")
{
    TestEnv env;

    const float wireWidth = 0.1f;
    const float outsideWidthDiagonal = 0.101f * std::sqrt(2.f);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f, 0.f), wireWidth) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.f, 0.49f), wireWidth) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f, 0.f), wireWidth) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.f, -0.49f), wireWidth) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.26f, 0.49f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.26f, 0.49f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f, -0.26f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f, 0.26f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.26f, -0.49f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.26f, -0.49f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f, -0.26f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f, 0.26f), wireWidth) == false);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f, 0.49f), wireWidth) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f, -0.49f), wireWidth) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f, 0.49f), wireWidth) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f, -0.49f), wireWidth) == true);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f, 0.49f - outsideWidthDiagonal), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f - outsideWidthDiagonal, 0.49f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f, -0.49f + outsideWidthDiagonal), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(0.49f - outsideWidthDiagonal, -0.49f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f, -0.49f + outsideWidthDiagonal), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f + outsideWidthDiagonal, -0.49f), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f, 0.49f - outsideWidthDiagonal), wireWidth) == false);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::All, glm::vec2(-0.49f + outsideWidthDiagonal, 0.49f), wireWidth) == false);
}

SYSTEM_TEST_CASE("Check hitbox for wire diagonals overlapping into neighbour cells")
{
    TestEnv env;
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(0.51f, 0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NE, glm::vec2(0.49f, 0.51f)) == true);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(-0.51f, 0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::NW, glm::vec2(-0.49f, 0.51f)) == true);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(-0.51f, -0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SW, glm::vec2(-0.49f, -0.51f)) == true);

    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(0.51f, -0.49f)) == true);
    CHECK(IsHoveringWireSegment(env, xg::EWireDirection::SE, glm::vec2(0.49f, -0.51f)) == true);
}

SYSTEM_TEST_CASE("While hovering a cog box -> set the cog entity to the cog")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog = world.entity();

    {
        auto& cogComponent = cog.ensure<xg::CogComponent>();
        cogComponent.m_CogId = s_OneCellCog;
        cogComponent.m_Transform = xc::ITransform{ .m_Translation = glm::ivec2(1, 2) };

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(cog);

        world.get_mut<xg::RenderSettings>().m_CogBoxSize = 100.f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.51f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.49f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);
}

SYSTEM_TEST_CASE("While hovering a non cog entity -> set the cog entity to null")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity entity = world.entity();

    {
        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(entity);

        world.get_mut<xg::RenderSettings>().m_CogBoxSize = 100.f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == flecs::entity::null());
}

SYSTEM_TEST_CASE("While hovering a cog entity cell, inside/outside of cog box -> set the cog entity only when inside the cog box")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog = world.entity();
    {
        auto& cogComponent = cog.ensure<xg::CogComponent>();
        cogComponent.m_CogId = s_OneCellCog;
        cogComponent.m_Transform = xc::ITransform{ .m_Translation = glm::ivec2(1, 2) };

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(cog);

        world.get_mut<xg::RenderSettings>().m_CogBoxSize = 0.25f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.25f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.26f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == flecs::entity::null());

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 1.74f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == flecs::entity::null());

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 1.76f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);
}

SYSTEM_TEST_CASE("While hovering a cog with size>1, inside/outside of cog box -> set the cog entity only when inside the cog box")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog = world.entity();
    {
        auto& cogComponent = cog.ensure<xg::CogComponent>();
        cogComponent.m_CogId = s_TestCog1;
        cogComponent.m_Transform = xc::ITransform{ .m_Translation = glm::ivec2(1, 2) };

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(cog);
        gridAttachmentsComponent.m_Map[glm::ivec2(2, 2)].m_Entities.push_back(cog);
        gridAttachmentsComponent.m_Map[glm::ivec2(3, 2)].m_Entities.push_back(cog);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 3)].m_Entities.push_back(cog);
        gridAttachmentsComponent.m_Map[glm::ivec2(2, 3)].m_Entities.push_back(cog);
        gridAttachmentsComponent.m_Map[glm::ivec2(3, 3)].m_Entities.push_back(cog);

        world.get_mut<xg::RenderSettings>().m_CogBoxSize = 0.25f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.75f, 3.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.74f, 3.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == flecs::entity::null());

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.f, 1.75f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.f, 1.74f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == flecs::entity::null());

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.f, 3.25f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.f, 3.26f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == flecs::entity::null());
}

SYSTEM_TEST_CASE("While hovering another entity and a cog box -> set the cog entity to the cog")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog = world.entity();

    {
        auto& cogComponent = cog.ensure<xg::CogComponent>();
        cogComponent.m_CogId = s_OneCellCog;
        cogComponent.m_Transform = xc::ITransform{ .m_Translation = glm::ivec2(1, 2) };

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(world.entity());
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(cog);

        world.get_mut<xg::RenderSettings>().m_CogBoxSize = 100.f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Cog == cog);
}

SYSTEM_TEST_CASE("While hovering a single wire, cell has multiple overlapping wires -> set the wire entity to the wire that is hovered")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire1 = world.entity();
    flecs::entity wire2 = world.entity();
    {
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(2, 2));

        wire2.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));
        wire2.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 3));

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::E);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::N);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire1);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire2);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.f;
        world.get_mut<xg::UISettings>().m_WireHoverWidth = 0.1f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.4f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire1);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.4f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire2);
}

SYSTEM_TEST_CASE("While hovering multiple wire segments -> set the wire entity to highest entity handle")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire1 = world.entity();
    flecs::entity wire2 = world.entity();
    {
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(0, 2));
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(2, 2));

        wire2.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(0, 0));
        wire2.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 1));
        wire2.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 3));

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::E);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::W);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::N);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::S);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire2);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire1);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.f;
        world.get_mut<xg::UISettings>().m_WireHoverWidth = 0.1f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire2);
}

SYSTEM_TEST_CASE("While hovering multiple wire checkpoints -> set the wire entity to highest entity handle")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire2 = world.entity();
    flecs::entity wire1 = world.entity();
    {
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(0, 2));
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));

        wire2.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasWireCheckpoint = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::S);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire1);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire2);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.2f;
        world.get_mut<xg::UISettings>().m_WireHoverWidth = 0.1f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.15f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire1);
}

SYSTEM_TEST_CASE("While hovering a wire segment with a higher checkpoint in the same cell -> set the wire entity to the segment")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity wire1 = world.entity();
    flecs::entity wire2 = world.entity();
    {
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(0, 2));
        wire1.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));

        wire2.ensure<xg::WireComponent>().m_Checkpoints.push_back(glm::ivec2(1, 2));

        auto& gridAttachmentsComponent = world.get_mut<xg::GridAttachmentsComponent>();
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_HasWireCheckpoint = true;
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_WireDirectionFlags.Raise(xg::EWireDirection::S);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire1);
        gridAttachmentsComponent.m_Map[glm::ivec2(1, 2)].m_Entities.push_back(wire2);

        world.get_mut<xg::RenderSettings>().m_WireDotOuterRadius = 0.1f;
        world.get_mut<xg::UISettings>().m_WireHoverWidth = 0.2f;
    }

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(0.85f, 2.f);
    env.Update();
    CHECK(world.get<xg::UIHoverComponent>().m_Wire == wire1);
}
