#include "UIDragPreviewSystem.h"

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "FlecsGame.h"
#include "GridHelpers.h"
#include "UIDragPreviewChanged.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIRotateComponent.h"
#include "WorldMouseComponent.h"

void xg::UIDragPreviewSystem::Update(flecs::world& world)
{
    if (world.has<xg::UIDragPreviewChanged>())
    {
        world.remove<xg::UIDragPreviewChanged>();
    }

    const auto& previewAddingCog = world.get<const xg::UIPreviewAddingCogComponent>();

    if (previewAddingCog.m_HoverCogId)
    {
        const auto& cogMap = world.get<const xg::CogMap>();
        const xg::CogPrototype* cog = cogMap.Get(previewAddingCog.m_HoverCogId);
        const glm::vec2 offset = -glm::vec2(cog->GetSize() - glm::ivec2(1, 1));

        bool found = false;
        world.each([&](const xg::UIDragPreviewComponent&, xg::UIDragPreviewCogComponent& previewCog)
            {
                world.get_mut<xg::UIDragPreviewMovement>().m_Translation = previewAddingCog.m_PreviewPosition + offset;
                if (previewCog.m_CogId != previewAddingCog.m_HoverCogId)
                {
                    previewCog.m_CogId = previewAddingCog.m_HoverCogId;
                    world.add<xg::UIDragPreviewChanged>();
                }
                found = true;
            });

        if (!found)
        {
            flecs::entity dragEntity = xg::CreateEntity(world);
            dragEntity.add<xg::UIDragPreviewComponent>();
            dragEntity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = previewAddingCog.m_HoverCogId;
            world.get_mut<xg::UIDragPreviewMovement>().m_Translation = previewAddingCog.m_PreviewPosition + offset;
            world.add<xg::UIDragPreviewChanged>();
        }
    }
    else if (previewAddingCog.m_AddCogId)
    {
        if (world.query<xg::UIDragPreviewComponent, xg::UIDragPreviewCogComponent>().count() == 0)
        {
            flecs::entity dragEntity = xg::CreateEntity(world);
            dragEntity.add<xg::UIDragPreviewComponent>();
            dragEntity.ensure<xg::UIDragPreviewCogComponent>().m_CogId = previewAddingCog.m_AddCogId;
            world.add<xg::UIDragPreviewChanged>();
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
                world.add<xg::UIDragPreviewChanged>();
            });
        world.defer_end();

        auto& dragPreview = world.get_mut<xg::UIDragPreviewMovement>();
        dragPreview.m_Translation = glm::vec2(0.f);
        dragPreview.m_Rotation = xc::Rotation90(0);
    }
}