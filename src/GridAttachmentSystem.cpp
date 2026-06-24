#include "GridAttachmentSystem.h"

#include <flecs/flecs.h>

#include "Core/IAABB.h"
#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "GridAttachmentsComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageRemovedComponent.h"

void xg::GridAttachmentSystem::Update(flecs::world& world)
{
    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    const auto& cogMap = world.get<xg::CogMap>();

    world.each([&](flecs::entity entity, xg::OnStageAddedComponent, const xg::CogComponent& cog)
        {
            const auto* cogPrototype = cogMap.Get(cog.m_CogId);
            for (int x = 0; x <= cogPrototype->GetSize().x - 1; ++x)
            {
                for (int y = 0; y <= cogPrototype->GetSize().y - 1; ++y)
                {
                    const glm::ivec2 local = glm::ivec2(x, y);
                    auto& attachments = map[cog.m_Transform.Apply(local)];
                    attachments.m_Entities.push_back(entity);
                    attachments.m_HasNode = cogPrototype->GetWireNodes().contains(local);
                }
            }
        });

    world.each([&](xg::OnStageRemovedComponent, const xg::CogComponent& cog)
        {
            const auto* cogPrototype = cogMap.Get(cog.m_CogId);
            for (int x = 0; x <= cogPrototype->GetSize().x - 1; ++x)
            {
                for (int y = 0; y <= cogPrototype->GetSize().y - 1; ++y)
                {
                    // TODO when adding wires we cannot just remove here
                    map.erase(cog.m_Transform.Apply(glm::ivec2(x, y)));
                }
            }
        });
}