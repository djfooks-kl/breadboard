#include "GridAttachmentSystem.h"

#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "GridAttachmentsComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageRemovedComponent.h"

void xg::GridAttachmentSystem::Update(flecs::world& world)
{
    auto& map = world.get_mut<xg::GridAttachmentsComponent>().m_Map;
    const auto& cogMap = world.get<xg::CogMap>();
    for (const flecs::entity entity : world.get<xg::OnStageAddedComponent>().m_Entities)
    {
        const auto* cog = entity.try_get<xg::CogComponent>();
        // TODO test no cog
        const auto* cogPrototype = cogMap.Get(cog->m_CogId);
        for (int x = 0; x <= cogPrototype->GetSize().x - 1; ++x)
        {
            for (int y = 0; y <= cogPrototype->GetSize().y - 1; ++y)
            {
                glm::ivec2 local = glm::ivec2(x, y);
                auto& attachments = map[cog->m_Transform.Apply(local)];
                attachments.m_Entities.push_back(entity);
                attachments.m_HasNode = cogPrototype->GetWireNodes().contains(local);
            }
        }
    }

    for (const flecs::entity entity : world.get<xg::OnStageRemovedComponent>().m_Entities)
    {
        const auto* cog = entity.try_get<xg::CogComponent>();
        // TODO test no cog
        const auto* cogPrototype = cogMap.Get(cog->m_CogId);
        for (int x = 0; x <= cogPrototype->GetSize().x - 1; ++x)
        {
            for (int y = 0; y <= cogPrototype->GetSize().y - 1; ++y)
            {
                map.erase(cog->m_Transform.Apply(glm::ivec2(x, y)));
            }
        }
    }
}