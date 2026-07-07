#include "GridAttachmentSystem.h"

#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Core/IAABB.h"
#include "Core/Range.h"
#include "EntityHash.h"
#include "GridAttachmentsComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageRemovedComponent.h"
#include "WireComponent.h"
#include "WireHelpers.h"

namespace
{
    void AddWire(flecs::entity entity, const xg::WireComponent& wire, xg::GridAttachmentsMap& inout_map)
    {
        glm::ivec2 prev = wire.m_Checkpoints[0];
        for (glm::ivec2 next : wire.m_Checkpoints)
        {
            inout_map[next].m_HasWireDot = true;

            const glm::ivec2 dir = xg::GetIDirection(prev, next);
            const int distance = xg::ChebyshevDistance(prev, next);
            const xg::EWireDirection forward = xg::WireDirectionToFlag(dir);
            const xg::EWireDirection backward = xg::WireDirectionToFlag(-dir);
            for (int i = 0; i <= distance; ++i)
            {
                const glm::ivec2 p = prev + dir * i;
                auto& attachments = inout_map[p];
                if (!xc::RangeContains(attachments.m_Entities, entity))
                {
                    attachments.m_Entities.push_back(entity);
                }
                if (i < distance)
                {
                    attachments.m_WireDirectionFlags |= forward;
                }
                if (i > 0)
                {
                    attachments.m_WireDirectionFlags |= backward;
                }
            }
            prev = next;
        }
    }
}

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

    world.each([&](flecs::entity entity, xg::OnStageRemovedComponent, const xg::CogComponent& cog)
        {
            const auto* cogPrototype = cogMap.Get(cog.m_CogId);
            for (int x = 0; x <= cogPrototype->GetSize().x - 1; ++x)
            {
                for (int y = 0; y <= cogPrototype->GetSize().y - 1; ++y)
                {
                    const glm::ivec2 p = cog.m_Transform.Apply(glm::ivec2(x, y));
                    auto& attachments = map[p];
                    std::erase(attachments.m_Entities, entity);
                    if (attachments.m_Entities.empty())
                    {
                        map.erase(p);
                    }
                    else
                    {
                        attachments.m_HasNode = false;
                    }
                }
            }
        });

    world.each([&](flecs::entity entity, xg::OnStageAddedComponent, const xg::WireComponent& wire)
        {
            AddWire(entity, wire, map);
        });

    world.each([&](flecs::entity entity, xg::OnStageRemovedComponent, const xg::WireComponent& wire)
        {
            std::unordered_set<flecs::entity> dirty;

            glm::ivec2 prev = wire.m_Checkpoints[0];
            for (glm::ivec2 next : wire.m_Checkpoints)
            {
                const glm::ivec2 dir = xg::GetIDirection(prev, next);
                const int distance = xg::ChebyshevDistance(prev, next);
                for (int i = 0; i <= distance; ++i)
                {
                    const glm::ivec2 p = prev + dir * i;
                    auto& attachments = map[p];
                    std::erase(attachments.m_Entities, entity);
                    if (attachments.m_Entities.empty())
                    {
                        map.erase(p);
                    }
                    else
                    {
                        for (flecs::entity otherEntity : attachments.m_Entities)
                        {
                            dirty.insert(otherEntity);
                        }
                        attachments.m_HasWireDot = false;
                        attachments.m_WireDirectionFlags.ClearAll();
                    }
                }
                prev = next;
            }

            for (const flecs::entity& entityToReAdd : dirty)
            {
                if (const auto* reAddWireComponent = entityToReAdd.try_get<const xg::WireComponent>())
                {
                    AddWire(entityToReAdd, *reAddWireComponent, map);
                }
            }
        });
}