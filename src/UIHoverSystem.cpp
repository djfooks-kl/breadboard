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
#include "WireComponent.h"
#include "WireHelpers.h"
#include "WorldMouseComponent.h"

namespace
{
    bool IsInsideWireSegmentHitbox(const glm::vec2& prev, const glm::vec2& next, glm::vec2 mousePos, const float wireWidth)
    {
        const glm::vec2 localEnd = next - prev;
        const glm::vec2 localMousePos = mousePos - prev;
        const float wireLength = glm::length(localEnd);
        const glm::vec2 direction = localEnd / wireLength;
        const glm::vec2 tangent(direction.y, -direction.x);
        const float widthDistance = std::abs(glm::dot(localMousePos, tangent));
        const float distance = dot(localMousePos, direction);
        return distance >= 0.f && distance <= wireLength && widthDistance <= wireWidth;
    }

    bool IsHoveringWireEntity(
        flecs::world& world,
        const flecs::entity& entity,
        const glm::vec2& worldMouse)
    {
        const auto* wireComponent = entity.try_get<xg::WireComponent>();
        if (!wireComponent)
            return false;

        if (wireComponent->m_Checkpoints.size() <= 1)
        {
            const float distance = glm::distance(worldMouse, glm::vec2(wireComponent->m_Checkpoints[0]));
            if (distance <= world.get<xg::RenderSettings>().m_WireDotOuterRadius)
            {
                return true;
            }
        }
        else
        {
            const float wireDotRadius = world.get<xg::RenderSettings>().m_WireDotOuterRadius;
            const float wireWidth = world.get<xg::UISettings>().m_WireHoverWidth;
            const std::vector<glm::ivec2>& checkpoints = wireComponent->m_Checkpoints;
            glm::vec2 prev = checkpoints[0];
            if (glm::distance(prev, worldMouse) <= wireDotRadius)
                return true;
            for (int i = 0; i < checkpoints.size(); ++i)
            {
                const glm::vec2 next = checkpoints[i];
                if (glm::distance(next, worldMouse) <= wireDotRadius)
                    return true;
                if (IsInsideWireSegmentHitbox(prev, next, worldMouse, wireWidth))
                {
                    return true;
                }
                prev = next;
            }
        }
        return false;
    }

    void UpdateWireEntity(
        flecs::world& world,
        const xg::GridAttachmentsMap& gridAttachmentsMap,
        const glm::vec2& worldMouse,
        const glm::vec2& cell,
        xg::UIHoverComponent& uiHoverComponent)
    {
        auto itr = gridAttachmentsMap.find(cell);
        if (itr != gridAttachmentsMap.end())
        {
            const auto& entities = itr->second.m_Entities;
            for (flecs::entity entity : entities)
            {
                if (IsHoveringWireEntity(world, entity, worldMouse))
                {
                    if (entity.id() > uiHoverComponent.m_Wire.id())
                    {
                        uiHoverComponent.m_Wire = entity;
                    }
                }
            }
        }
    }
}

void xg::UIHoverSystem::Update(flecs::world& world)
{
    const glm::vec2& worldMouse = world.get<xg::WorldMouseComponent>().m_Position;
    const xg::GridAttachmentsMap& gridAttachmentsMap = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    glm::vec2 mouseCell = xg::SnapToGrid(worldMouse);

    auto& uiHoverComponent = world.get_mut<xg::UIHoverComponent>();
    uiHoverComponent.m_Node = false;
    uiHoverComponent.m_Wire = flecs::entity::null();
    uiHoverComponent.m_Cog = flecs::entity::null();

    const glm::vec2 mouseOffset = worldMouse - mouseCell;

    auto itr = gridAttachmentsMap.find(mouseCell);
    if (itr != gridAttachmentsMap.end())
    {
        if (itr->second.m_HasNode)
        {
            const float distance = glm::length(mouseOffset);
            uiHoverComponent.m_Node = distance <= world.get<xg::RenderSettings>().m_NodeOuterRadius;
        }
        const auto& entities = itr->second.m_Entities;
        for (flecs::entity entity : entities)
        {
            if (const auto* cogComponent = entity.try_get<xg::CogComponent>())
            {
                const glm::vec2 hitSize(world.get<xg::RenderSettings>().m_CogBoxSize);

                const auto& cogMap = world.get<xg::CogMap>();
                const auto* cogPrototype = cogMap.Get(cogComponent->m_CogId);
                const xc::IAABB cellAABB = xc::IAABB::FromTransformAndSize(cogComponent->m_Transform, cogPrototype->GetSize());
                const xc::AABB hitAABB{
                    .m_Min = glm::vec2(cellAABB.m_Min) - hitSize,
                    .m_Max = glm::vec2(cellAABB.m_Max) + hitSize };

                if (hitAABB.Contains(worldMouse))
                {
                    uiHoverComponent.m_Cog = entity;
                }
            }
        }
    }

    UpdateWireEntity(world, gridAttachmentsMap, worldMouse, mouseCell, uiHoverComponent);

    // check the closest edge for diagonal wires that might overlap this cell
    const glm::vec2 closestEdge = std::abs(mouseOffset.x) > std::abs(mouseOffset.y) ?
        glm::vec2(glm::sign(mouseOffset.x), 0.f) :
        glm::vec2(0.f, glm::sign(mouseOffset.y));
    UpdateWireEntity(world, gridAttachmentsMap, worldMouse, mouseCell + closestEdge, uiHoverComponent);

    uiHoverComponent.m_Entity = uiHoverComponent.m_Wire.is_valid() ? uiHoverComponent.m_Wire : uiHoverComponent.m_Cog;
}