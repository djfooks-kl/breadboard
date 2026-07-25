#include "UIAddWireSystem.h"

#include "FlecsGame.h"
#include "UIAddWireComponent.h"
#include "UIPreviewCreateWireComponent.h"
#include "UIWireSegmentsComponent.h"
#include "UIWireValidComponent.h"

void xg::UIAddWireSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](flecs::entity entity, const xg::UIAddWireComponent&)
        {
            entity.destruct();
        });
    world.defer_end();

    if (world.get<xg::UIPreviewCreateWireComponent>().m_Create)
    {
        world.defer_begin();
        world.each([&](const xg::UIWireSegmentsComponent& wirePreview, const xg::UIWireValidComponent& validComponent)
            {
                if (validComponent.m_Valid)
                {
                    flecs::entity entity = xg::CreateEntity(world);
                    auto& addWire = entity.ensure<xg::UIAddWireComponent>();
                    addWire.m_Checkpoints = wirePreview.m_Checkpoints;
                }
            });
        world.defer_end();
    }
}