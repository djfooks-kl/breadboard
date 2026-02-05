#pragma once

#include <unordered_map>
#include <vector>
#include <flecs/flecs.h>
#include <glm/ext/vector_int2.hpp>

template<>
struct std::hash<glm::ivec2>
{
    std::size_t operator()(const glm::ivec2& p) const
    {
        const std::size_t h1 = std::hash<int>{}(p.x);
        const std::size_t h2 = std::hash<int>{}(p.y);
        return h1 ^ (h2 << 1);
    }
};

namespace xg
{
    using GridAttachmentsMap = std::unordered_map<glm::ivec2, std::vector<flecs::entity>>;
}
