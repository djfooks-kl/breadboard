#pragma once

#include <flecs/flecs.h>
#include <source_location>

namespace xg
{
	flecs::entity CreateEntity(
		flecs::world& world,
		const std::source_location location = std::source_location::current());
}