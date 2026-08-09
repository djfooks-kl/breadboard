#include "HoverVFXSystem.h"

#include "HoverVFXComponent.h"
#include "UIHoverComponent.h"
#include "UISettings.h"
#include "WorldMouseComponent.h"

void xg::HoverVFXSystem::Update(flecs::world& world, float deltaTime)
{
    const glm::vec2& worldMouse = world.get<xg::WorldMouseComponent>().m_Position;
    const auto& uiHoverComponent = world.get<xg::UIHoverComponent>();
    world.defer_begin();
    world.each([&](
        flecs::entity entity,
        xg::HoverVFXComponent& hoverVFX)
        {
            if (entity == uiHoverComponent.m_Entity)
            {
                if (hoverVFX.m_WasHovering)
                {
                    hoverVFX.m_InDuration += deltaTime;
                }
                else
                {
                    hoverVFX.m_WasHovering = true;
                    hoverVFX.m_InDuration = 0.f;
                    hoverVFX.m_InPosition = worldMouse;
                    hoverVFX.m_OutDuration = 0.f;
                    hoverVFX.m_OutPosition = glm::vec2(0.f);
                }
                return;
            }

            if (hoverVFX.m_WasHovering)
            {
                hoverVFX.m_WasHovering = false;
                hoverVFX.m_OutPosition = worldMouse;
            }
            else
            {
                hoverVFX.m_OutDuration += deltaTime;
                if (hoverVFX.m_OutDuration > world.get<xg::UISettings>().m_HoverDuration)
                {
                    entity.remove<xg::HoverVFXComponent>();
                }
            }
        });
    world.defer_end();

    if (uiHoverComponent.m_Entity && !uiHoverComponent.m_Entity.has<xg::HoverVFXComponent>())
    {
        uiHoverComponent.m_Entity.ensure<xg::HoverVFXComponent>().m_InPosition = worldMouse;
    }
}