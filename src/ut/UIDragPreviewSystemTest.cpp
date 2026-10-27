#include "BreadTest.h"
#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "SelectedComponent.h"
#include "UIDragPreviewChanged.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIDragPreviewSystem.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIPreviewMovingComponent.h"
#include "UIRotateComponent.h"
#include "WorldMouseComponent.h"

#define SYSTEM_TEST_CASE(description) TEST_CASE("xg::UIDragPreviewSystem - " description, "[xg::UIDragPreviewSystem]")

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
            m_World.ensure<xg::UIDragPreviewMovement>();
            m_World.ensure<xg::UIPreviewAddingCogComponent>();
            m_World.ensure<xg::UIPreviewMovingComponent>();
            m_World.ensure<xg::UIRotateComponent>();
            m_World.ensure<xg::WorldMouseComponent>();

            auto& cogMap = m_World.ensure<xg::CogMap>();
            cogMap.Register<Size1Cog>();
            cogMap.Register<LongCog>();
        }

        void Update()
        {
            xg::UIDragPreviewSystem::Update(m_World);

            m_World.get_mut<xg::UIRotateComponent>().m_RotationDirection = 0;
        }

        flecs::world m_World;
    };
}

SYSTEM_TEST_CASE("While hovering -> show a preview at the given position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = s_Size1Cog;
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(1.f, 2.f);
    }

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&, const xg::UIDragPreviewCogComponent& previewCog)
        {
            createdEntity = entity;
            CHECK(previewCog.m_CogId == s_Size1Cog);
        });
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(1.f, 2.f));
    CHECK(world.has<xg::UIDragPreviewChanged>());

    env.Update();
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);

    world.get_mut<xg::UIPreviewAddingCogComponent>().m_PreviewPosition = glm::vec2(2.f, 3.f);
    env.Update();

    REQUIRE(world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&, const xg::UIDragPreviewCogComponent& previewCog)
        {
            CHECK(createdEntity == entity);
            CHECK(previewCog.m_CogId == s_Size1Cog);
        });
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(2.f, 3.f));
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);

    world.get_mut<xg::UIPreviewAddingCogComponent>().m_HoverCogId = s_LongCog;
    env.Update();

    CHECK(world.has<xg::UIDragPreviewChanged>());
    REQUIRE(world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&, const xg::UIDragPreviewCogComponent& previewCog)
        {
            CHECK(createdEntity == entity);
            CHECK(previewCog.m_CogId == s_LongCog);
        });

    world.get_mut<xg::UIPreviewAddingCogComponent>().m_PreviewPosition = glm::vec2(4.f, 4.f);
    world.get_mut<xg::UIPreviewAddingCogComponent>().m_HoverCogId = s_Size1Cog;
    env.Update();
    CHECK(world.has<xg::UIDragPreviewChanged>());
}

SYSTEM_TEST_CASE("While hovering, with > 1 size cog -> show a preview with the cog bottom right corner at the given position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = s_LongCog;
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(0.f, 0.f);
    }

    env.Update();

    flecs::entity createdEntity;
    CHECK(world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 1);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(-1.f, -2.f));
    CHECK(world.has<xg::UIDragPreviewChanged>());

    world.get_mut<xg::UIPreviewAddingCogComponent>().m_PreviewPosition = glm::vec2(4.f, 4.f);
    env.Update();
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(3.f, 2.f));
}

SYSTEM_TEST_CASE("Stop hovering -> remove the drag preview")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = s_Size1Cog;
    }

    env.Update();

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = xg::CogResourceId();
    }

    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 0);
    CHECK(world.has<xg::UIDragPreviewChanged>());
}

SYSTEM_TEST_CASE("Hovering ends and cog is being added -> show a preview at the mouse position instead")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = s_Size1Cog;
        uiPreviewAddingCog.m_PreviewPosition = glm::vec2(1.f, 2.f);
    }

    env.Update();

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_HoverCogId = xg::CogResourceId();
        uiPreviewAddingCog.m_AddCogId = s_Size1Cog;
    }
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(2.6f, 4.4f);

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&, const xg::UIDragPreviewCogComponent& previewCog)
        {
            createdEntity = entity;
            CHECK(previewCog.m_CogId == s_Size1Cog);
        });
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(3, 4));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(2.6f, 4.4f));
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(5.1f, 6.1f);
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(5, 6));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(5.1f, 6.1f));
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
}

SYSTEM_TEST_CASE("Cog is being added (without hovering first) -> show a preview at the mouse position")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_AddCogId = s_Size1Cog;
    }
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(3.f, 4.f);

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&, const xg::UIDragPreviewCogComponent& previewCog)
        {
            createdEntity = entity;
            CHECK(previewCog.m_CogId == s_Size1Cog);
        });
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(3, 4));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(3.f, 4.f));
    CHECK(world.has<xg::UIDragPreviewChanged>());
}

SYSTEM_TEST_CASE("Rotation changes while previewing -> Update the rotation")
{
    TestEnv env;
    flecs::world world = env.m_World;

    {
        auto& uiPreviewAddingCog = world.get_mut<xg::UIPreviewAddingCogComponent>();
        uiPreviewAddingCog.m_AddCogId = s_Size1Cog;
    }

    env.Update();

    flecs::entity createdEntity;
    REQUIRE(world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 1);
    world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&, const xg::UIDragPreviewCogComponent&)
        {
            createdEntity = entity;
        });

    world.get_mut<xg::UIRotateComponent>().m_RotationDirection = 1;
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(1));
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);

    world.get_mut<xg::UIRotateComponent>().m_RotationDirection = 0;
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(1));
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);

    world.get_mut<xg::UIRotateComponent>().m_RotationDirection = -1;
    env.Update();

    CHECK(world.count<xg::UIDragPreviewComponent>() == 1);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
}

SYSTEM_TEST_CASE("Rotation changes while previewing then stop -> Movement should be reset")
{
    TestEnv env;
    flecs::world world = env.m_World;

    world.get_mut<xg::UIPreviewAddingCogComponent>().m_AddCogId = s_Size1Cog;
    env.Update();

    world.get_mut<xg::UIRotateComponent>().m_RotationDirection = 1;
    env.Update();

    world.get_mut<xg::UIPreviewAddingCogComponent>().m_AddCogId = {};
    env.Update();

    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(0.f));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(0));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
}

SYSTEM_TEST_CASE("While moving, dragged entity is in selected list -> move all selected entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog1 = world.entity();
    cog1.add<xg::SelectedComponent>();
    flecs::entity cog2 = world.entity();
    cog2.add<xg::SelectedComponent>();

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = cog2;
    world.get_mut<xg::UIPreviewMovingComponent>().m_GrabbedPosition = glm::vec2(1.75f, 2.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 5.75f);

    env.Update();

    CHECK(cog1.has<xg::UIDragPreviewComponent>());
    CHECK(cog2.has<xg::UIDragPreviewComponent>());
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(-0.75f, 3.75f));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(-1, 4));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.has<xg::UIDragPreviewChanged>());

    env.Update();
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
}

SYSTEM_TEST_CASE("While moving, dragged entity is not in selected list -> move only the dragged entity")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog1 = world.entity();
    cog1.add<xg::SelectedComponent>();
    flecs::entity cog2 = world.entity();
    cog2.add<xg::SelectedComponent>();
    flecs::entity cog3 = world.entity();

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = cog3;
    world.get_mut<xg::UIPreviewMovingComponent>().m_GrabbedPosition = glm::vec2(1.75f, 2.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 5.75f);

    env.Update();

    CHECK(cog1.has<xg::UIDragPreviewComponent>() == false);
    CHECK(cog2.has<xg::UIDragPreviewComponent>() == false);
    CHECK(cog3.has<xg::UIDragPreviewComponent>());
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(-0.75f, 3.75f));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(-1, 4));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.has<xg::UIDragPreviewChanged>());

    env.Update();
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
}

SYSTEM_TEST_CASE("While moving, dragged entity starts as null -> do nothing")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog1 = world.entity();
    cog1.add<xg::SelectedComponent>();

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = flecs::entity::null();
    world.get_mut<xg::UIPreviewMovingComponent>().m_GrabbedPosition = glm::vec2(1.75f, 2.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(1.f, 5.75f);

    env.Update();

    CHECK(cog1.has<xg::UIDragPreviewComponent>() == false);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(0.f, 0.f));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(0, 0));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);

    env.Update();
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
}

SYSTEM_TEST_CASE("While moving, translation/rotation changes -> move all selected entities")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog1 = world.entity();
    cog1.add<xg::SelectedComponent>();
    flecs::entity cog2 = world.entity();
    cog2.add<xg::SelectedComponent>();

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = cog1;
    world.get_mut<xg::UIPreviewMovingComponent>().m_GrabbedPosition = glm::vec2(1.75f, 2.f);
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(3.f, 5.f);

    env.Update();

    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(3.f, 5.75f);
    env.Update();
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(1.25f, 3.75f));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(1, 4));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));

    world.get_mut<xg::UIRotateComponent>().m_RotationDirection = 1;
    env.Update();

    // 1. rotate around origin by rotation
    // 2. rotatedGrabbed = rotate the grabbed pos by rotation
    // 3. set translate to rotatedGrabbed->mouse

    // rotatedGrabbed = (2, -1.75)
    // translate = (3, 5.75) - (2, -1.75) = (1, 7.5)

    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(1.f, 7.5f));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(1, 8));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(1));
}

SYSTEM_TEST_CASE("Moving stops -> remove the UIDragPreviewComponent")
{
    TestEnv env;
    flecs::world world = env.m_World;

    flecs::entity cog1 = world.entity();
    cog1.add<xg::SelectedComponent>();
    flecs::entity cog2 = world.entity();
    cog2.add<xg::SelectedComponent>();

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = cog2;
    world.get_mut<xg::WorldMouseComponent>().m_Position = glm::vec2(3.f, 5.f);

    env.Update();

    world.get_mut<xg::UIRotateComponent>().m_RotationDirection = 1;
    env.Update();

    world.get_mut<xg::UIPreviewMovingComponent>().m_Dragging = flecs::entity::null();
    env.Update();

    CHECK(cog1.has<xg::UIDragPreviewComponent>() == false);
    CHECK(cog2.has<xg::UIDragPreviewComponent>() == false);
    CHECK(world.get<xg::UIDragPreviewMovement>().m_FreeTranslation == glm::vec2(0.f));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Translation == glm::ivec2(0, 0));
    CHECK(world.get<xg::UIDragPreviewMovement>().m_Transform.m_Rotation == xc::Rotation90(0));
    CHECK(world.has<xg::UIDragPreviewChanged>());

    env.Update();
    CHECK(world.has<xg::UIDragPreviewChanged>() == false);
}