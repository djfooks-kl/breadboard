#include "UIPreviewWireSystem.h"

#include <flecs/flecs.h>

#include "UIPreviewAddingWireComponent.h"
#include "UIPreviewWireComponent.h"
#include "WorldMouseComponent.h"

void xg::UIPreviewWireSystem::Update(flecs::world& world)
{
    if (world.get<xg::UIPreviewAddingWireComponent>().m_Active)
    {
        if (world.count<xg::UIPreviewWireComponent>() == 0)
        {
            flecs::entity entity = world.entity();
            entity.ensure<xg::UIPreviewWireComponent>().m_Start = world.get<xg::WorldMouseComponent>().m_Cell;
        }
    }
    else
    {
        world.defer_begin();
        world.each([&](flecs::entity entity, const xg::UIPreviewWireComponent&)
            {
                entity.destruct();
            });
        world.defer_end();
    }
}