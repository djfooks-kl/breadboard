#include "GridAttachmentSystem.h"

#include <flecs/flecs.h>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandMoveGroupComponent.h"
#include "Core/IAABB.h"
#include "Core/Range.h"
#include "EntityHash.h"
#include "GridAttachmentsComponent.h"
#include "GridHelpers.h"
#include "OnStageAddedComponent.h"
#include "OnStageRemovedComponent.h"
#include "WireComponent.h"
#include "WireHelpers.h"

namespace
{
    void AddCog(
        flecs::entity entity,
        const xg::CogMap& cogMap,
        const xg::CogComponent& cog,
        xg::GridAttachmentsMap& inout_map)
    {
        const auto* cogPrototype = cogMap.Get(cog.m_CogId);
        for (int x = 0; x <= cogPrototype->GetSize().x - 1; ++x)
        {
            for (int y = 0; y <= cogPrototype->GetSize().y - 1; ++y)
            {
                const glm::ivec2 local = glm::ivec2(x, y);
                auto& attachments = inout_map[cog.m_Transform.Apply(local)];
                attachments.m_Entities.push_back(entity);
                attachments.m_HasNode = cogPrototype->GetWireNodes().contains(local);
            }
        }
    }

    void DeleteCog(
        flecs::entity entity,
        const xg::CogMap& cogMap,
        const xg::CogComponent& cog,
        const xc::ITransform& transform,
        xg::GridAttachmentsMap& inout_map)
    {
        const auto* cogPrototype = cogMap.Get(cog.m_CogId);
        for (int x = 0; x <= cogPrototype->GetSize().x - 1; ++x)
        {
            for (int y = 0; y <= cogPrototype->GetSize().y - 1; ++y)
            {
                const glm::ivec2 p = transform.Apply(glm::ivec2(x, y));
                auto& attachments = inout_map[p];
                std::erase(attachments.m_Entities, entity);
                if (attachments.m_Entities.empty())
                {
                    inout_map.erase(p);
                }
                else
                {
                    attachments.m_HasNode = false;
                }
            }
        }
    }

    void AddWire(flecs::entity entity, const xg::WireComponent& wire, xg::GridAttachmentsMap& inout_map)
    {
        glm::ivec2 prev = wire.m_Checkpoints[0];
        for (glm::ivec2 next : wire.m_Checkpoints)
        {
            inout_map[next].m_HasWireCheckpoint = true;

            const glm::ivec2 dir = xg::GetIDirection(prev, next);
            const int distance = xg::ChebyshevDistance(prev, next);
            const xg::EWireDirection forward = xg::WireDirectionToFlag(dir);
            const xg::EWireDirection backward = xg::WireDirectionToFlag(-dir);
            for (int i = 0; i <= distance; ++i)
            {
                const glm::ivec2 p = prev + dir * i;
                auto& attachments = inout_map[p];
                xc::AppendUnique(attachments.m_Entities, entity);

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

    void DeleteWire(
        flecs::entity entity,
        const xg::WireComponent& wire,
        const xc::ITransform& transform,
        xg::GridAttachmentsMap& inout_map,
        std::unordered_set<flecs::entity>& inout_dirtyMap)
    {
        glm::ivec2 prev = transform.Apply(wire.m_Checkpoints[0]);
        for (glm::ivec2 localNext : wire.m_Checkpoints)
        {
            const glm::ivec2 next = transform.Apply(localNext);
            const glm::ivec2 dir = xg::GetIDirection(prev, next);
            const int distance = xg::ChebyshevDistance(prev, next);
            for (int i = 0; i <= distance; ++i)
            {
                const glm::ivec2 p = prev + dir * i;
                auto& attachments = inout_map[p];
                std::erase(attachments.m_Entities, entity);
                if (attachments.m_Entities.empty())
                {
                    inout_map.erase(p);
                }
                else
                {
                    for (flecs::entity otherEntity : attachments.m_Entities)
                    {
                        inout_dirtyMap.insert(otherEntity);
                    }
                    attachments.m_HasWireCheckpoint = false;
                    attachments.m_WireDirectionFlags.ClearAll();
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
            AddCog(entity, cogMap, cog, map);
        });

    world.each([&](flecs::entity entity, xg::OnStageRemovedComponent, const xg::CogComponent& cog)
        {
            DeleteCog(entity, cogMap, cog, cog.m_Transform, map);
        });

    world.each([&](
        const xg::command::MoveGroupComponent& moveGroup,
        const xg::command::ExecuteComponent&)
        {
            std::unordered_set<flecs::entity> dirty;
            for (const flecs::entity& entity : moveGroup.m_Entities)
            {
                if (entity.has<xg::OnStageAddedComponent>())
                    continue;

                if (const auto* cog = entity.try_get<const xg::CogComponent>())
                {
                    DeleteCog(entity, cogMap, *cog, moveGroup.m_Transform.Inverse() * cog->m_Transform, map);
                    AddCog(entity, cogMap, *cog, map);
                }
                else if (const auto* wire = entity.try_get<const xg::WireComponent>())
                {
                    DeleteWire(entity, *wire, moveGroup.m_Transform.Inverse(), map, dirty);
                    AddWire(entity, *wire, map);
                }
            }

            for (const flecs::entity& entityToReAdd : dirty)
            {
                if (const auto* reAddWireComponent = entityToReAdd.try_get<const xg::WireComponent>())
                {
                    AddWire(entityToReAdd, *reAddWireComponent, map);
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

            DeleteWire(entity, wire, xc::ITransform::s_Identy, map, dirty);

            for (const flecs::entity& entityToReAdd : dirty)
            {
                if (const auto* reAddWireComponent = entityToReAdd.try_get<const xg::WireComponent>())
                {
                    AddWire(entityToReAdd, *reAddWireComponent, map);
                }
            }
        });
}