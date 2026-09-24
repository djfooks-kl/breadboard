#include "UIDragValidationSystem.h"

#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Core/IAABB.h"
#include "GridAttachmentsComponent.h"
#include "GridHelpers.h"
#include "GridSizeComponent.h"
#include "UIDragPreviewCogComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragPreviewMovement.h"
#include "UIDragValidComponent.h"

namespace
{
    bool GridHasCog(const xg::GridAttachmentsMap& map, const glm::ivec2& p)
    {
        auto itr = map.find(p);
        if (itr == map.end())
            return false;

        for (const flecs::entity& entity : itr->second.m_Entities)
        {
            if (entity.has<xg::CogComponent>())
                return true;
        }
        return false;
    }
}

void xg::UIDragValidationSystem::Update(flecs::world& world)
{
    const auto& attachmentsMap = world.get<xg::GridAttachmentsComponent>().m_Map;
    const glm::ivec2& gridSize = world.get<xg::GridSizeComponent>().m_Size;
    const auto& previewMovement = world.get<xg::UIDragPreviewMovement>();

    bool valid = true;
    world.each([&](const xg::UIDragPreviewCogComponent& dragPreview)
        {
            const auto* cogPrototype = world.get<xg::CogMap>().Get(dragPreview.m_CogId);
            const xc::ITransform transform{
                .m_Translation = xg::SnapToGrid(previewMovement.m_Translation),
                .m_Rotation = previewMovement.m_Rotation };
            const xc::IAABB aabb = xc::IAABB::FromTransformAndSize(transform, cogPrototype->GetSize());
            aabb.ForEachCellUntil(
                [&](const glm::ivec2& p)
                {
                    if (GridHasCog(attachmentsMap, p) ||
                        p.x < 0 ||
                        p.y < 0 ||
                        p.x > gridSize.x ||
                        p.y > gridSize.y)
                    {
                        valid = false;
                        return true;
                    }
                    return false;
                });
            return valid;
        });

    world.get_mut<xg::UIDragValidComponent>().m_Valid = valid;
}