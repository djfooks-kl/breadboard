#include "OnStageTestHelpers.h"

#include "OnStageComponent.h"

flecs::entity xg::ut::CreateOnStageEntity(flecs::world& world)
{
    flecs::entity entity = world.entity();
    entity.add<xg::OnStageComponent>();
    return entity;
}
