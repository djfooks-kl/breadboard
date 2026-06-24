#include "WireSystem.h"

#include <flecs/flecs.h>

#include "WireComponent.h"
#include "Command/CommandAddWireComponent.h"
#include "Command/CommandEntityComponent.h"
#include "Command/CommandExecuteComponent.h"

void xg::WireSystem::Update(flecs::world& world)
{
    world.defer_begin();
    world.each([&](
        const xg::command::AddWireComponent& addWire,
        const xg::command::EntityComponent& commandEntity,
        const xg::command::ExecuteComponent&)
        {
            flecs::entity wireEntity = commandEntity.m_Entity;
            auto& wire = wireEntity.ensure<xg::WireComponent>();
            wire.m_Checkpoints = addWire.m_Checkpoints;
        });
    world.defer_end();
}
