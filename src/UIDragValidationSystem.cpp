#include "UIDragValidationSystem.h"

#include <flecs/flecs.h>

#include "Cogs/CogMap.h"
#include "GridAttachmentsComponent.h"
#include "GridSizeComponent.h"
#include "UIDragPreviewComponent.h"
#include "UIDragValidComponent.h"
#include "Cogs/CogHelpers.h"

void xg::UIDragValidationSystem::Update(flecs::world& world)
{
    const auto& attachmentsMap = world.get<xg::GridAttachmentsComponent>().m_Map;
    const glm::ivec2& gridSize = world.get<xg::GridSizeComponent>().m_Size;

    bool valid = true;
    world.each([&](const xg::UIDragPreviewComponent& dragPreview)
        {
            const auto* cogPrototype = world.get<xg::CogMap>().Get(dragPreview.m_CogId);
            xg::cog::ForEachCellUntil(dragPreview.m_Position, cogPrototype->GetSize(), dragPreview.m_Rotation,
                [&](const glm::ivec2& p)
                {
                    if (attachmentsMap.contains(p) ||
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