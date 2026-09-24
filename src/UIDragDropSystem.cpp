#include "UIDragDropSystem.h"

#include "FlecsGame.h"
#include "GridHelpers.h"
#include "UIAddCogComponent.h"
#include "UIDraggingDropComponent.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIDragValidComponent.h"

void xg::UIDragDropSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIAddCogComponent&)
        {
            entity.destruct();
        });
    world.defer_end();

    if (world.get<xg::UIDraggingDropComponent>().m_Drop &&
        world.get<xg::UIDragValidComponent>().m_Valid)
    {
        const auto& dragMovement = world.get<xg::UIDragPreviewMovement>();

        world.defer_begin();
        world.each([&](const xg::UIDragPreviewComponent&, const xg::UIDragPreviewCogComponent& previewCog)
            {
                flecs::entity entity = xg::CreateEntity(world);
                auto& addCog = entity.ensure<xg::UIAddCogComponent>();
                addCog.m_CogId = previewCog.m_CogId;
                addCog.m_Transform = xc::ITransform{ xg::SnapToGrid(dragMovement.m_Translation), dragMovement.m_Rotation };
            });
        world.defer_end();
    }
}