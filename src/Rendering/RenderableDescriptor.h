#pragma once

#include <unordered_map>
#include "Rendering/RenderableResourceId.h"

namespace xg
{
    enum class ERenderableMode : uint8_t
    {
        InGame = 1,
        Preview = 2
    };

    struct RenderableDescriptor
    {
        xg::RenderableResourceId m_ResourceId;
        xg::ERenderableMode m_Mode = ERenderableMode::InGame;

        bool operator==(const xg::RenderableDescriptor& other) const = default;
    };
}

template<>
struct std::hash<xg::RenderableDescriptor>
{
    std::size_t operator()(const xg::RenderableDescriptor& v) const
    {
        const std::size_t h1 = std::hash<int>{}(v.m_ResourceId);
        const std::size_t h2 = std::hash<int>{}(static_cast<int>(v.m_Mode));
        return h1 ^ (h2 << 1);
    }
};
