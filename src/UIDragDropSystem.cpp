#include "UIDragDropSystem.h"

#include "FlecsGame.h"
#include "GridHelpers.h"
#include "UIAddCogComponent.h"
#include "UIDragDeleteGroupComponent.h"
#include "UIDraggingDropComponent.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIDragValidComponent.h"
#include "UIMoveGroupComponent.h"
#include "UIPreviewMovingComponent.h"

void xg::UIDragDropSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIAddCogComponent&)
        {
            entity.destruct();
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIMoveGroupComponent&)
        {
            entity.destruct();
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIDragDeleteGroupComponent&)
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
                addCog.m_Transform = dragMovement.m_Transform;
            });
        world.defer_end();
    }

    if (!world.get<xg::UIPreviewMovingComponent>().m_Dragging)
    {
        const auto& dragMovement = world.get<xg::UIDragPreviewMovement>();

        std::vector<flecs::entity> movingEntities;

        auto addQuery = world.query_builder<const xg::UIDragPreviewComponent>().without<const xg::UIDragPreviewCogComponent>();
        addQuery.build().each([&](flecs::entity movingEntity, const xg::UIDragPreviewComponent&)
            {
                movingEntities.push_back(movingEntity);
            });

        if (movingEntities.size() > 0)
        {
            flecs::entity entity = xg::CreateEntity(world);
            if (world.get<xg::UIDragValidComponent>().m_Valid)
            {
                auto& moveGroup = entity.ensure<xg::UIMoveGroupComponent>();
                moveGroup.m_Entities = std::move(movingEntities);
                moveGroup.m_Transform = dragMovement.m_Transform;
            }
            else
            {
                entity.ensure<xg::UIDragDeleteGroupComponent>().m_Entities = std::move(movingEntities);
            }
        }
    }
}