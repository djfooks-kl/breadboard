#include "SelectionBoxSystem.h"

#include <flecs/flecs.h>
#include <glm/geometric.hpp>

#include "CogComponent.h"
#include "Core/GeometryHelpers.h"
#include "Core/OBB.h"
#include "CogHelpers.h"
#include "Cogs/CogMap.h"
#include "Core/AABB.h"
#include "Core/GLFWLib.h"
#include "OnStageComponent.h"
#include "RenderSettings.h"
#include "SelectionBoxComponent.h"
#include "UISelectComponent.h"
#include "WireComponent.h"
#include "WireHelpers.h"
#include "WorldMouseComponent.h"

void xg::SelectionBoxSystem::Update(flecs::world& world)
{
    auto& entities = world.get_mut<xg::SelectionBoxComponent>().m_Entities;
    entities.clear();

    const auto& uiSelect = world.get<xg::UISelectComponent>();
    if (!uiSelect.m_SelectBox || !uiSelect.m_BoxStart)
        return;

    const glm::vec2& worldMouse = world.get<xg::WorldMouseComponent>().m_Position;
    const auto& cogMap = world.get<xg::CogMap>();
    const auto& renderSettings = world.get<xg::RenderSettings>();

    const xc::AABB box = xc::AABB::FromPoints(worldMouse, uiSelect.m_BoxStart.value());
    const xc::AABBCenterExtents boxCenterExtents = box.ToCenterExtents();

    world.each([&](flecs::entity entity, xg::OnStageComponent, const xg::CogComponent& cogComponent)
        {
            const xg::CogPrototype* cog = cogMap.Get(cogComponent.m_CogId);
            const xc::AABB aabb = xg::GetCogAABB(cog->GetSize(), cogComponent.m_Transform, renderSettings);
            if (box.Overlaps(aabb))
            {
                entities.push_back(entity);
            }
        });

    world.each([&](flecs::entity entity, xg::OnStageComponent, const xg::WireComponent& wireComponent)
        {
            if (wireComponent.m_Checkpoints.empty())
                return;

            if (xc::CircleOverlapsAABB(wireComponent.m_Checkpoints[0], renderSettings.m_WireDotOuterRadius, box))
            {
                entities.push_back(entity);
                return;
            }
            const glm::ivec2* prev = &wireComponent.m_Checkpoints[0];
            for (int i = 1; i < wireComponent.m_Checkpoints.size(); ++i)
            {
                const glm::ivec2& next = wireComponent.m_Checkpoints[i];

                if (xc::CircleOverlapsAABB(next, renderSettings.m_WireDotOuterRadius, box))
                {
                    entities.push_back(entity);
                    return;
                }

                const xc::OBB segment = xg::GetWireSegmentOBB(*prev, next, renderSettings.m_WireOuterWidth);
                if (xc::AABBOverlapsOBB(boxCenterExtents, segment))
                {
                    entities.push_back(entity);
                    return;
                }
                prev = &next;
            }
        });
}