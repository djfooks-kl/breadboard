#include "WireSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandAddWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandMoveGroupComponent.h"
#include "WireComponent.h"

void xg::WireSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](
        const xg::command::AddWireComponent& addWire,
        const xg::command::EntityComponent& commandEntity,
        const xg::command::ExecuteComponent&)
        {
            flecs::entity wireEntity = commandEntity.m_Entity;
            if (wireEntity.has<xg::WireComponent>())
                return;

            auto& wire = wireEntity.ensure<xg::WireComponent>();
            wire.m_Checkpoints = addWire.m_Checkpoints;
        });
    world.defer_end();

    world.each([&](
        const xg::command::MoveGroupComponent& moveGroup,
        const xg::command::ExecuteComponent&)
        {
            for (const auto& entity : moveGroup.m_Entities)
            {
                auto* wire = entity.try_get_mut<xg::WireComponent>();
                if (!wire)
                    continue;

                for (glm::ivec2& checkpoint : wire->m_Checkpoints)
                {
                    checkpoint = moveGroup.m_Transform.Apply(checkpoint);
                }
            }
        });
}
