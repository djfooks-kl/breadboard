#include "SelectionSystem.h"

#include <flecs/flecs.h>
#include <glm/geometric.hpp>

#include "Core/GLFWLib.h"
#include "MappedInputComponent.h"
#include "OnStageRemovedComponent.h"
#include "SelectedComponent.h"
#include "SelectionBoxComponent.h"
#include "SelectionChangedComponent.h"
#include "UISelectComponent.h"

namespace
{
    void RemoveAllSelected(flecs::world& world)
    {
        world.defer_begin();
        world.each([&](
            flecs::entity entity,
            const xg::SelectedComponent&)
            {
                entity.remove<xg::SelectedComponent>();
            });
        world.defer_end();
    }
}

void xg::SelectionSystem::Update(flecs::world& world)
{
    world.remove<xg::SelectionChangedComponent>();
    const auto& uiSelect = world.get_mut<xg::UISelectComponent>();
    const flecs::entity& selectEntity = uiSelect.m_SelectEntity;

    bool anyChanges = false;
    world.defer_begin();
    world.each([&](
        flecs::entity entity,
        const xg::OnStageRemovedComponent,
        const xg::SelectedComponent)
        {
            entity.remove<xg::SelectedComponent>();
            anyChanges = true;
        });
    world.defer_end();

    if (uiSelect.m_SelectBox)
    {
        if (!world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::Multiselect))
        {
            if (world.count<xg::SelectedComponent>() > 0)
            {
                RemoveAllSelected(world);
                anyChanges = true;
            }
        }

        if (world.get<const xg::SelectionBoxComponent>().m_Entities.size() > 0)
        {
            anyChanges = true;
        }
        for (flecs::entity entity : world.get<const xg::SelectionBoxComponent>().m_Entities)
        {
            entity.add<xg::SelectedComponent>();
        }
    }
    else if (selectEntity)
    {
        if (world.get<xg::MappedInputComponent>().m_KeyDown.contains(xg::EMappedInput::Multiselect))
        {
            if (selectEntity.has<xg::SelectedComponent>())
            {
                selectEntity.remove<xg::SelectedComponent>();
            }
            else
            {
                selectEntity.add<xg::SelectedComponent>();
            }
            anyChanges = true;
        }
        else if (world.count<xg::SelectedComponent>() != 1 || !selectEntity.has<xg::SelectedComponent>())
        {
            RemoveAllSelected(world);
            selectEntity.add<xg::SelectedComponent>();
            anyChanges = true;
        }
    }

    if (anyChanges)
    {
        world.add<xg::SelectionChangedComponent>();
    }
}