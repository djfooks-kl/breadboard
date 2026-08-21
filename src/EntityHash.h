#pragma once

#include <flecs/flecs.h>
#include <functional>

namespace std
{
    template<>
    struct hash<flecs::entity>
    {
        std::size_t operator()(const flecs::entity& entity) const noexcept
        {
            return std::hash<flecs::entity_t>{}(entity.id());
        }
    };
}