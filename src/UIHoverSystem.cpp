#include "UIHoverSystem.h"

#include <flecs/flecs.h>
#include <glm/geometric.hpp>

#include "GridAttachmentsComponent.h"
#include "RenderSettings.h"
#include "UIHoverComponent.h"
#include "WorldMouseComponent.h"

void xg::UIHoverSystem::Update(flecs::world& world)
{
    const glm::vec2& worldMouse = world.get<xg::WorldMouseComponent>().m_Position;
    const xg::GridAttachmentsMap& gridAttachmentsMap = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    glm::vec2 mouseCell(std::round(worldMouse.x), std::round(worldMouse.y));
    bool hovering = false;
    auto itr = gridAttachmentsMap.find(mouseCell);
    if (itr != gridAttachmentsMap.end())
    {
        if (itr->second.m_HasNode)
        {
            float distance = glm::distance(worldMouse, mouseCell);
            hovering = distance <= world.get<xg::RenderSettings>().m_NodeOuterRadius;
        }
    }
    world.get_mut<xg::UIHoverComponent>().m_Node = hovering;
}