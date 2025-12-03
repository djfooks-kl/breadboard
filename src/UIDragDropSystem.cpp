#include "UIDragDropSystem.h"

#include <flecs/flecs.h>

#include "UIDragPreviewComponent.h"
#include "UIDraggingDropComponent.h"
#include "UIAddCogComponent.h"

void xg::UIDragDropSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIAddCogComponent&)
        {
            entity.remove<xg::UIAddCogComponent>();
        });
    world.defer_end();

    if (!world.get<xg::UIDraggingDropComponent>().m_Drop)
        return;

    world.defer_begin();
    world.each([&](const xg::UIDragPreviewComponent& dragPreview)
        {
            flecs::entity entity = world.entity();
            auto& addCog = entity.ensure<xg::UIAddCogComponent>();
            addCog.m_CogId = dragPreview.m_CogId;
            addCog.m_Position = dragPreview.m_Position;
            addCog.m_Rotation = dragPreview.m_Rotation;
        });
    world.defer_end();
}