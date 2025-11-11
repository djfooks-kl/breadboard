#include "UIDragPreviewSystem.h"

#include <flecs/flecs.h>

#include "UIDragPreviewComponent.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIRotateComponent.h"
#include "WorldMouseComponent.h"

void xg::UIDragPreviewSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIPreviewAddingCogComponent& previewAddingCog)
        {
            auto& dragPreview = entity.ensure<xg::UIDragPreviewComponent>();
            dragPreview.m_CogId = previewAddingCog.m_CogId;
            if (previewAddingCog.m_Hovering)
            {
                dragPreview.m_PreviewPosition = previewAddingCog.m_PreviewPosition;
            }
            else
            {
                dragPreview.m_PreviewPosition = world.get<xg::WorldMouseComponent>().m_Position;
                dragPreview.m_Position.x = static_cast<int>(std::round(dragPreview.m_PreviewPosition.x));
                dragPreview.m_Position.y = static_cast<int>(std::round(dragPreview.m_PreviewPosition.y));
            }
        });
    world.defer_end();

    world.each([&](xg::UIDragPreviewComponent& dragPreview)
        {
            dragPreview.m_Rotation.RotateRight(world.get<xg::UIRotateComponent>().m_RotationDirection);
        });
}