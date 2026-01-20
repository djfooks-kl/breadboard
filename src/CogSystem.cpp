#include "CogSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddCogComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "CogCreatedComponent.h"
#include "CogComponent.h"

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
                cog.m_Position = addCog.m_Position;
                cog.m_Rotation = addCog.m_Rotation;
            }
        });
    world.defer_end();
}
