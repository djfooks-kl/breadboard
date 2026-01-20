#pragma once

namespace flecs
{
    struct world;
}

namespace xg::OnStageSystem
{
    void Update(flecs::world& world);
}