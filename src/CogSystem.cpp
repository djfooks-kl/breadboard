#include "CogSystem.h"

#include <flecs/flecs.h>

#include "CogComponent.h"
#include "CogCreatedComponent.h"
#include "CogNodesComponent.h"
#include "Cogs/CogMap.h"
#include "Command/CommandAddCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"

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
}
