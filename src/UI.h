#pragma once

namespace flecs
{
    struct world;
}

namespace xg
{
    class UI
    {
    public:
        void Draw(flecs::world& world);

    private:
        void DrawMenu(flecs::world& world);

        void DrawDebugInfo(flecs::world& world);

        bool m_ShowMenuBar = false;

        bool m_DebugInfoOpen = false;
    };
}