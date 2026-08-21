#include "MappedInputSystem.h"

#include <flecs/flecs.h>

#include "Core/GLFWLib.h"
#include "InputComponent.h"
#include "MappedInputComponent.h"
#include "KeybindingSettings.h"

namespace
{
    xg::EMappedInput GetBinding(const int key, const std::unordered_map<int, xg::EMappedInput>& bindings)
    {
        auto itr = bindings.find(key);
        if (itr == bindings.end())
            return xg::EMappedInput::Invalid;

        return itr->second;
    }
}

void xg::MappedInputSystem::Update(flecs::world& world)
{
    auto& mappedInput = world.get_mut<xg::MappedInputComponent>();

    const auto& inputs = world.get<xg::InputComponent>();
    const auto& bindings = world.get<xg::KeybindingSettings>().m_Bindings;
    if (bindings.empty())
        return;

    mappedInput.m_KeyDown.clear();
    for (const int key : inputs.m_KeyDown)
    {
        const xg::EMappedInput binding = GetBinding(key, bindings);
        if (binding != xg::EMappedInput::Invalid)
        {
            mappedInput.m_KeyDown.insert(binding);
        }
    }

    mappedInput.m_KeyPress.clear();
    for (const int key : inputs.m_KeyPress)
    {
        const xg::EMappedInput binding = GetBinding(key, bindings);
        if (binding != xg::EMappedInput::Invalid)
        {
            mappedInput.m_KeyPress.insert(binding);
        }
    }

    mappedInput.m_KeyRelease.clear();
    for (const int key : inputs.m_KeyRelease)
    {
        const xg::EMappedInput binding = GetBinding(key, bindings);
        if (binding != xg::EMappedInput::Invalid)
        {
            mappedInput.m_KeyRelease.insert(binding);
        }
    }
}