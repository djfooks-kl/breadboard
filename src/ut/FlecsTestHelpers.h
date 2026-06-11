#pragma once

namespace xc::ut
{
    template<typename... Components>
    std::vector<flecs::entity> CollectEntities(flecs::query<Components...> query)
    {
        std::vector<flecs::entity> results;
        query.each([&](flecs::entity e, Components&...) {
            results.push_back(e);
        });
        return std::move(results);
    }
}