#include "UIHoverSystem.h"

#include <flecs/flecs.h>
#include <glm/geometric.hpp>

#include "Core/AABB.h"
#include "Core/IAABB.h"
#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "GridAttachmentsComponent.h"
#include "RenderSettings.h"
#include "UIHoverComponent.h"
#include "WorldMouseComponent.h"
#include "GridHelpers.h"

void xg::UIHoverSystem::Update(flecs::world& world)
{
    const glm::vec2& worldMouse = world.get<xg::WorldMouseComponent>().m_Position;
    const xg::GridAttachmentsMap& gridAttachmentsMap = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    glm::vec2 mouseCell = xg::SnapToGrid(worldMouse);

    auto& uiHoverComponent = world.get_mut<xg::UIHoverComponent>();
    uiHoverComponent.m_Node = false;
    uiHoverComponent.m_Cog = flecs::entity::null();

    auto itr = gridAttachmentsMap.find(mouseCell);
    if (itr != gridAttachmentsMap.end())
    {
        if (itr->second.m_HasNode)
        {
            const float distance = glm::distance(worldMouse, mouseCell);
            uiHoverComponent.m_Node = distance <= world.get<xg::RenderSettings>().m_NodeOuterRadius;
        }

        const auto& entities = itr->second.m_Entities;
        if (!entities.empty())
        {
            if (const auto* cogComponent = entities[0].try_get<xg::CogComponent>())
            {
                const glm::vec2 hitSize(world.get<xg::RenderSettings>().m_CogBoxSize);

                const auto& cogMap = world.get<xg::CogMap>();
                const auto* cogPrototype = cogMap.Get(cogComponent->m_CogId);
                xc::IAABB cellAABB = xc::IAABB::FromTransformAndSize(cogComponent->m_Transform, cogPrototype->GetSize());
                xc::AABB hitAABB{
                    .m_Min = glm::vec2(cellAABB.m_Min) - hitSize,
                    .m_Max = glm::vec2(cellAABB.m_Max) + hitSize };

                if (hitAABB.Contains(worldMouse))
                {
                    uiHoverComponent.m_Cog = entities[0];
                }
            }
        }
    }
}