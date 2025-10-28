#include "Command/CommandListSystem.h"

#include <flecs/flecs.h>

#include "Command/CommandToQueueComponent.h"
#include "Command/CommandExecuteComponent.h"
#include "Command/CommandListComponent.h"
#include "Command/CommandUndoComponent.h"
#include "UIUndoComponent.h"
#include "UIRedoComponent.h"

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

    world.each([&](flecs::entity entity, const xg::command::ToQueueComponent& toQueue)
        {
            if (list.m_UndoCount == 0)
            {
                list.m_UndoHeadIndex = 0;
            }
            else
            {
                list.m_UndoHeadIndex = (list.m_UndoHeadIndex + 1) % list.m_Commands.size();
            }
            list.m_HeadIndex = list.m_UndoHeadIndex;

            list.m_UndoCount = std::min(list.m_UndoCount + 1, static_cast<int>(list.m_Commands.size()));
            list.m_Count = list.m_UndoCount;

            flecs::entity& oldCommand = list.m_Commands[list.m_HeadIndex];
            if (oldCommand.is_alive())
            {
                oldCommand.get<xg::command::UndoComponent>().m_Undo.destruct();
                oldCommand.destruct();
            }

            list.m_Commands[list.m_HeadIndex] = entity;
            entity.add<xg::command::ExecuteComponent>();
            entity.ensure<xg::command::UndoComponent>().m_Undo = toQueue.m_Undo;
        });

    world.each([&](const xg::UIUndoComponent)
        {
            if (list.m_UndoCount == 0)
                return;

            flecs::entity undo = list.m_Commands[list.m_UndoHeadIndex].get<const xg::command::UndoComponent>().m_Undo;
            undo.add<xg::command::ExecuteComponent>();

            --list.m_UndoCount;

            if (list.m_UndoCount > 0)
            {
                --list.m_UndoHeadIndex;
                if (list.m_UndoHeadIndex < 0)
                    list.m_UndoHeadIndex = static_cast<int>(list.m_Commands.size()) - 1;
            }
        });

    world.each([&](const xg::UIRedoComponent)
        {
            if (list.m_UndoCount == list.m_Count)
                return;

            if (list.m_UndoCount > 0)
            {
                list.m_UndoHeadIndex = (list.m_UndoHeadIndex + 1) % list.m_Commands.size();
            }
            ++list.m_UndoCount;
            list.m_Commands[list.m_UndoHeadIndex].add<xg::command::ExecuteComponent>();
        });

    world.defer_end();
}