#include "CogSystem.h"

#include <flecs/flecs.h>

#include "CogComponent.h"
#include "CogCreatedComponent.h"
#include "CogNodesComponent.h"
#include "Cogs/CogMap.h"
#include "Command/CommandAddCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandMoveGroupComponent.h"

void xg::CogSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](
        flecs::entity entity,
        const xg::CogCreatedComponent)
        {
            entity.remove<xg::CogCreatedComponent>();
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](
        const xg::command::AddCogComponent& addCog,
        const xg::command::EntityComponent& commandEntity,
        const xg::command::ExecuteComponent&)
        {
            flecs::entity cogEntity = commandEntity.m_Entity;
            if (!cogEntity.has<xg::CogComponent>())
            {
                cogEntity.add<xg::CogCreatedComponent>();
                auto& cog = cogEntity.ensure<xg::CogComponent>();
                cog.m_CogId = addCog.m_CogId;
                cog.m_Transform = addCog.m_Transform;

                auto& cogNodes = cogEntity.ensure<xg::CogNodesComponent>();
                const xg::CogPrototype* prototype = world.get<const xg::CogMap>().Get(cog.m_CogId);
                cogNodes.m_Nodes.reserve(prototype->GetWireNodes().size());
                for (const glm::ivec2& node : prototype->GetWireNodes())
                {
                    cogNodes.m_Nodes.push_back(addCog.m_Transform.Apply(node));
                }
            }
        });
    world.defer_end();

    world.defer_begin();
    world.each([&](
        const xg::command::MoveGroupComponent& moveGroup,
        const xg::command::ExecuteComponent&)
        {
            for (const auto& entity : moveGroup.m_Entities)
            {
                auto* cog = entity.try_get_mut<xg::CogComponent>();
                if (!cog)
                    continue;

                cog->m_Transform = moveGroup.m_Transform * cog->m_Transform;

                auto& cogNodes = entity.get_mut<xg::CogNodesComponent>();
                const xg::CogPrototype* prototype = world.get<const xg::CogMap>().Get(cog->m_CogId);
                int i = 0;
                for (const glm::ivec2& node : prototype->GetWireNodes())
                {
                    cogNodes.m_Nodes[i] = cog->m_Transform.Apply(node);
                    ++i;
                }
            }
        });
    world.defer_end();
}
