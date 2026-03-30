#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>

#include "Rendering/IRenderer.h"
#include "Rendering/IWireRenderer.h"
#include "Rendering/RenderableResourceId.h"

namespace xg
{
    template<typename TRenderer>
    struct RendererMap
    {
        RendererMap() {}
        RendererMap(const RendererMap&) = delete;
        RendererMap& operator=(const RendererMap&) = delete;

        TRenderer* Get(const xg::RenderableResourceId& renderableResourceId)
        {
            auto itr = m_Map.find(renderableResourceId);
            if (itr != m_Map.end())
            {
                return itr->second.get();
            }
            return nullptr;
        }

        const TRenderer* Get(const xg::RenderableResourceId& renderableResourceId) const
        {
            auto itr = m_Map.find(renderableResourceId);
            if (itr != m_Map.end())
            {
                return itr->second.get();
            }
            return nullptr;
        }

        const std::unordered_map<xg::RenderableResourceId, std::unique_ptr<TRenderer>>& GetMap() const
        {
            return m_Map;
        }

        const std::vector<TRenderer*> GetOrder() const
        {
            return m_Order;
        }

        void Register(const xg::RenderableResourceId& resourceId, std::unique_ptr<TRenderer>&& renderer)
        {
            m_Order.push_back(renderer.get());
            m_Map[resourceId] = std::move(renderer);
        }

        void SortRenderers()
        {
            std::sort(m_Order.begin(), m_Order.end(), [](const TRenderer* a, const TRenderer* b)
            {
                return a->GetHeight() < b->GetHeight();
            });
        }

    private:
        std::unordered_map<xg::RenderableResourceId, std::unique_ptr<TRenderer>> m_Map;
        // highest first
        std::vector<TRenderer*> m_Order;
    };

    using CogRendererMap = RendererMap<xg::IRenderer>;
    using WireRendererMap = RendererMap<xg::IWireRenderer>;
}