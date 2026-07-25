#include "FlecsGame.h"
#include "GameComponent.h"

flecs::entity xg::CreateEntity(
	flecs::world& world,
	const std::source_location location)
{
	flecs::entity entity = world.entity();
	entity.ensure<xg::GameComponent>().m_CreatedAt = location;
	return entity;
}