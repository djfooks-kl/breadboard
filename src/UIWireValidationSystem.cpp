#include "UIWireValidationSystem.h"

#include <flecs/flecs.h>

#include "GridSizeComponent.h"
#include "UIWireSegmentsComponent.h"
#include "UIWireValidComponent.h"

void xg::UIWireValidationSystem::Update(flecs::world& world)
{
    const glm::ivec2& gridSize = world.get<xg::GridSizeComponent>().m_Size;

    world.defer_begin();
    auto addQuery = world.query_builder<const xg::UIWireSegmentsComponent>().without<xg::UIWireValidComponent>();
    addQuery.build().each([&](flecs::entity entity, const xg::UIWireSegmentsComponent&)
        {
            entity.ensure<xg::UIWireValidComponent>();
        });
    world.defer_end();

    world.each([&](const xg::UIWireSegmentsComponent& segments, xg::UIWireValidComponent& validComponent)
        {
            for (const glm::ivec2& checkpoint : segments.m_Checkpoints)
            {
                if (checkpoint.x < 0 ||
                    checkpoint.y < 0 ||
                    checkpoint.x > gridSize.x ||
                    checkpoint.y > gridSize.y)
                {
                    validComponent.m_Valid = false;
                    return;
                }
            }
            validComponent.m_Valid = true;
        });
}