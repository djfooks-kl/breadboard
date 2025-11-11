#include "UIDragDropSystem.h"

#include <flecs/flecs.h>

#include "UIDragPreviewComponent.h"
#include "UIDraggingDropComponent.h"
#include "UIAddCogComponent.h"

void xg::UIDragDropSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](
        flecs::entity entity,
        const xg::UIDragPreviewComponent& dragPreview,
        const xg::UIDraggingDropComponent&)
        {
            auto& addCog = entity.ensure<xg::UIAddCogComponent>();
            addCog.m_CogId = dragPreview.m_CogId;
            addCog.m_Position = dragPreview.m_Position;
            addCog.m_Rotation = dragPreview.m_Rotation;
        });
    world.defer_end();
}