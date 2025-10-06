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
        void DrawDebugMenu(flecs::world& world);
        void DrawDebugInfo(flecs::world& world);

        void DrawComponentMenu(flecs::world& world);

        bool m_ShowDebugMenuBar = false;
        bool m_DebugInfoOpen = false;

        bool m_ComponentMenuOpen = false;
        bool m_ComponentMenuMouseClose = false;
    };
}