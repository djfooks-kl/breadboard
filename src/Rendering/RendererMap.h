#pragma once

#include <memory>
#include <unordered_map>

#include "Rendering/IRenderer.h"
#include "Rendering/RenderableResourceId.h"

namespace xg
{
    struct RendererMap
    {
        RendererMap() {}
        RendererMap(const RendererMap&) = delete;
        RendererMap& operator=(const RendererMap&) = delete;

        xg::IRenderer* Get(const xg::RenderableResourceId& renderableResourceId);
        const xg::IRenderer* Get(const xg::RenderableResourceId& renderableResourceId) const;

        const std::unordered_map<xg::RenderableResourceId, std::unique_ptr<xg::IRenderer>>& GetMap() const { return m_Map; }

        void Register(const xg::RenderableResourceId& resourceId, std::unique_ptr<xg::IRenderer>&& renderer)
        {
            m_Map[resourceId] = std::move(renderer);
        }

    private:
        std::unordered_map<xg::RenderableResourceId, std::unique_ptr<xg::IRenderer>> m_Map;
    };
}