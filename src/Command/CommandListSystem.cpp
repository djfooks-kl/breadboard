#include "InputSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandToQueueComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandListComponent.h"
#include "Command/CommandListItemComponent.h"
#include "Command/CommandListSystem.h"

void xg::command::list_system::Update(flecs::world& world)
{
    auto& list = world.get_mut<xg::command::ListComponent>();

	world.defer_begin();
	world.each([](flecs::entity entity, xg::command::ExecuteComponent)
		{
			entity.remove<xg::command::ExecuteComponent>();
		});
	world.defer_end();

	world.defer_begin();

	auto query = world.query_builder<
		const xg::command::ToQueueComponent>();
	query.each([&](flecs::entity entity, const xg::command::ToQueueComponent& toQueue)
	{
		entity.add<xg::command::ExecuteComponent>();

		auto& listItem = entity.ensure<xg::command::ListItemComponent>();
		listItem.m_Undo = toQueue.m_Undo;

		flecs::entity oldHead = list.m_Head;
		if (oldHead.is_valid())
		{
			auto& oldHeadListItem = oldHead.get_mut<xg::command::ListItemComponent>();
			oldHeadListItem.m_Next = entity;

			listItem.m_Previous = oldHead;
		}

		list.m_Head = entity;
		if (!list.m_Tail.is_valid())
			list.m_Tail = entity;
		list.m_UndoHead = entity;
	});

	world.defer_end();
}