#include "UIDragPreviewSystem.h"

#include <flecs/flecs.h>

#include "UIDragPreviewComponent.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIRotateComponent.h"
#include "WorldMouseComponent.h"

namespace
{
    void UpdatePreviewAddingHover(
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
        out_dragPreview.m_Position = glm::ivec2(
            std::round(worldMouse.m_Position.x),
            std::round(worldMouse.m_Position.y));
        out_dragPreview.m_PreviewPosition = worldMouse.m_Position;
    }
}

void xg::UIDragPreviewSystem::Update(flecs::world& world)
{
    const auto& previewAddingCog = world.get<const xg::UIPreviewAddingCogComponent>();

    if (previewAddingCog.m_HoverCogId)
    {
        bool found = false;
        world.each([&](xg::UIDragPreviewComponent& dragPreview)
            {
                found = true;
                UpdatePreviewAddingHover(previewAddingCog, dragPreview);
            });

        if (!found)
        {
            flecs::entity dragEntity = world.entity();
            auto& dragPreview = dragEntity.ensure<xg::UIDragPreviewComponent>();
            UpdatePreviewAddingHover(previewAddingCog, dragPreview);
        }
    }
    else if (previewAddingCog.m_AddCogId)
    {
        const auto& worldMouse = world.get<const xg::WorldMouseComponent>();

        bool found = false;
        world.each([&](xg::UIDragPreviewComponent& dragPreview)
            {
                found = true;
                UpdatePreviewAdding(previewAddingCog, worldMouse, dragPreview);

                dragPreview.m_Rotation = world.get<xg::UIRotateComponent>().m_RotationDirection;
            });

        if (!found)
        {
            flecs::entity dragEntity = world.entity();
            auto& dragPreview = dragEntity.ensure<xg::UIDragPreviewComponent>();
            UpdatePreviewAdding(previewAddingCog, worldMouse, dragPreview);
        }
    }
    else
    {
        world.defer_begin();
        world.each([&](flecs::entity entity, const xg::UIDragPreviewComponent&)
            {
                entity.destruct();
            });
        world.defer_end();
    }
}