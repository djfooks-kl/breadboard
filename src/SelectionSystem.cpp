#include "SelectionSystem.h"

#include <flecs/flecs.h>
#include <glm/geometric.hpp>

#include "CogComponent.h"
#include "Cogs/CogMap.h"
#include "Core/AABB.h"
#include "Core/GLFWLib.h"
#include "Core/IAABB.h"
#include "GridHelpers.h"
#include "MappedInputComponent.h"
#include "RenderSettings.h"
#include "SelectedComponent.h"
#include "SelectionChangedComponent.h"
#include "UISelectComponent.h"
#include "UISettings.h"
#include "WireComponent.h"
#include "WireHelpers.h"
#include "WorldMouseComponent.h"

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

    if (uiSelect.m_SelectBox)
    {
        if (world.count<xg::SelectedComponent>() > 0)
        {
            RemoveAllSelected(world);
            world.add<xg::SelectionChangedComponent>();
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
            world.add<xg::SelectionChangedComponent>();
        }
        else if (world.count<xg::SelectedComponent>() != 1 || !selectEntity.has<xg::SelectedComponent>())
        {
            RemoveAllSelected(world);
            selectEntity.add<xg::SelectedComponent>();
            world.add<xg::SelectionChangedComponent>();
        }
    }
}