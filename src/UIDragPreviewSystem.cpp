#include "UIDragPreviewSystem.h"

#include "FlecsGame.h"
#include "GridHelpers.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIRotateComponent.h"
#include "WorldMouseComponent.h"

namespace
{
    /*void UpdatePreviewAddingHover(
        const xg::UIPreviewAddingCogComponent& previewAddingCog,
        xg::UIDragPreviewComponent& out_dragPreview)
    {
        out_dragPreview.m_CogId = previewAddingCog.m_HoverCogId;
        out_dragPreview.m_PreviewPosition = previewAddingCog.m_PreviewPosition;
    }

    void UpdatePreviewAdding(
        const xg::UIPreviewAddingCogComponent& previewAddingCog,
        const xg::WorldMouseComponent& worldMouse,
        xg::UIDragPreviewComponent& out_dragPreview)
    {
        out_dragPreview.m_CogId = previewAddingCog.m_AddCogId;
        out_dragPreview.m_Position = xg::SnapToGrid(worldMouse.m_Position);
        out_dragPreview.m_PreviewPosition = worldMouse.m_Position;
    }*/
}

void xg::UIDragPreviewSystem::Update(flecs::world& world)
{
    const auto& previewAddingCog = world.get<const xg::UIPreviewAddingCogComponent>();

    if (previewAddingCog.m_HoverCogId)
    {
        bool found = false;
        world.each([&](const xg::UIDragPreviewComponent&, xg::UIDragPreviewCogComponent& previewCog)
            {
                world.get_mut<xg::UIDragPreviewMovement>().m_Translation = previewAddingCog.m_PreviewPosition;
                previewCog.m_CogId = previewAddingCog.m_HoverCogId;
                found = true;
            });

        if (!found)
        {
            flecs::entity dragEntity = xg::CreateEntity(world);
            dragEntity.add<xg::UIDragPreviewComponent>();
            dragEntity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = previewAddingCog.m_HoverCogId;
            world.get_mut<xg::UIDragPreviewMovement>().m_Translation = previewAddingCog.m_PreviewPosition;
        }
    }
    else if (previewAddingCog.m_AddCogId)
    {
        if (world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 0)
        {
            flecs::entity dragEntity = xg::CreateEntity(world);
            dragEntity.add<xg::UIDragPreviewComponent>();
            dragEntity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = previewAddingCog.m_AddCogId;
        }

        const auto& worldMouse = world.get<const xg::WorldMouseComponent>();
        auto& dragPreview = world.get_mut<xg::UIDragPreviewMovement>();
        dragPreview.m_Translation = worldMouse.m_Position;
        dragPreview.m_Rotation += world.get<xg::UIRotateComponent>().m_RotationDirection;
    }
    else
    {
        world.defer_begin();
        world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&)
            {
                entity.destruct();
            });
        world.defer_end();

        auto& dragPreview = world.get_mut<xg::UIDragPreviewMovement>();
        dragPreview.m_Translation = glm::vec2(0.f);
        dragPreview.m_Rotation = xc::Rotation90(0);
    }
}