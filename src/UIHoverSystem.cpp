#include "UIHoverSystem.h"

#include <flecs/flecs.h>
#include <glm/geometric.hpp>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Core/AABB.h"
#include "Core/IAABB.h"
#include "GridAttachmentsComponent.h"
#include "GridHelpers.h"
#include "RenderSettings.h"
#include "UIHoverComponent.h"
#include "UISettings.h"
#include "WireHelpers.h"
#include "WorldMouseComponent.h"

namespace
{
    bool IsInsideWireHitbox(const glm::vec2& cell, glm::vec2 direction, const float wireWidth)
    {
        direction = glm::normalize(direction);
        glm::vec2 tangent(direction.y, -direction.x);
        float d = std::abs(glm::dot(cell, tangent));
        return dot(cell, direction) >= 0.f && d <= wireWidth;
    }
}

void xg::UIHoverSystem::Update(flecs::world& world)
{
    const glm::vec2& worldMouse = world.get<xg::WorldMouseComponent>().m_Position;
    const xg::GridAttachmentsMap& gridAttachmentsMap = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    glm::vec2 mouseCell = xg::SnapToGrid(worldMouse);

    auto& uiHoverComponent = world.get_mut<xg::UIHoverComponent>();
    uiHoverComponent.m_Node = false;
    uiHoverComponent.m_Wire = false;
    uiHoverComponent.m_Cog = flecs::entity::null();

    auto itr = gridAttachmentsMap.find(mouseCell);
    if (itr != gridAttachmentsMap.end())
    {
        if (itr->second.m_HasNode)
        {
            const float distance = glm::distance(worldMouse, mouseCell);
            uiHoverComponent.m_Node = distance <= world.get<xg::RenderSettings>().m_NodeOuterRadius;
        }

        if (xg::HasWireDot(itr->second))
        {
            const float distance = glm::distance(worldMouse, mouseCell);
            uiHoverComponent.m_Wire |= distance <= world.get<xg::RenderSettings>().m_WireDotOuterRadius;
        }

        const xg::TWireDirectionFlags flags = itr->second.m_WireDirectionFlags;
        if (flags.HasAny())
        {
            const float wireWidth = world.get<xg::UISettings>().m_WireHoverWidth;
            glm::vec2 cellPos = worldMouse - mouseCell;
            if (flags.Has(xg::EWireDirection::E))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(1.f, 0.f), wireWidth);
            }
            if (flags.Has(xg::EWireDirection::N))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(0.f, 1.f), wireWidth);
            }
            if (flags.Has(xg::EWireDirection::S))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(0.f, -1.f), wireWidth);
            }
            if (flags.Has(xg::EWireDirection::W))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(-1.f, 0.f), wireWidth);
            }
            if (flags.Has(xg::EWireDirection::NE))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(1.f, 1.f), wireWidth);
            }
            if (flags.Has(xg::EWireDirection::SE))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(1.f, -1.f), wireWidth);
            }
            if (flags.Has(xg::EWireDirection::SW))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(-1.f, -1.f), wireWidth);
            }
            if (flags.Has(xg::EWireDirection::NW))
            {
                uiHoverComponent.m_Wire |= IsInsideWireHitbox(cellPos, glm::vec2(-1.f, 1.f), wireWidth);
            }
        }

        const auto& entities = itr->second.m_Entities;
        if (!entities.empty())
        {
            for (flecs::entity entity : entities)
            {
                if (const auto* cogComponent = entity.try_get<xg::CogComponent>())
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
                        uiHoverComponent.m_Cog = entity;
                    }
                }
            }
        }
    }
}