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

    bool IsCogPositionValid(
        const xg::CogResourceId& cogId,
        const xc::ITransform& cogTransform,
        const xg::CogMap& cogMap,
        const xg::GridAttachmentsMap& attachmentsMap,
        const glm::ivec2& gridSize,
        const xg::UIDragPreviewMovement& previewMovement)
    {
        const auto* cogPrototype = cogMap.Get(cogId);
        const xc::IAABB aabb = xc::IAABB::FromTransformAndSize(previewMovement.m_Transform * cogTransform, cogPrototype->GetSize());

        bool valid = true;
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
    }
}

void xg::UIDragValidationSystem::Update(flecs::world& world)
{
    const auto& attachmentsMap = world.get<xg::GridAttachmentsComponent>().m_Map;
    const glm::ivec2& gridSize = world.get<xg::GridSizeComponent>().m_Size;
    const auto& previewMovement = world.get<xg::UIDragPreviewMovement>();
    const auto& cogMap = world.get<xg::CogMap>();

    bool valid = true;
    world.each([&](const xg::UIDragPreviewCogComponent& dragPreview)
        {
            if (!IsCogPositionValid(
                dragPreview.m_CogId,
                xc::ITransform::s_Identy,
                cogMap,
                attachmentsMap,
                gridSize,
                previewMovement))
            {
                valid = false;
            }
        });

    world.each([&](const xg::CogComponent& cogComponent, const xg::UIDragPreviewComponent&)
        {
            if (!IsCogPositionValid(
                cogComponent.m_CogId,
                cogComponent.m_Transform,
                cogMap,
                attachmentsMap,
                gridSize,
                previewMovement))
            {
                valid = false;
            }
        });

    world.get_mut<xg::UIDragValidComponent>().m_Valid = valid;
}