#pragma once

#include <functional>
#include <flecs/flecs.h>

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