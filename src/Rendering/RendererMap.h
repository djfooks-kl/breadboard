#pragma once

#include <memory>
#include <unordered_map>

#include "Rendering/IRenderer.h"
#include "Rendering/RenderableDescriptor.h"

namespace xg
{
    struct RendererMap
    {
        RendererMap() {}
        RendererMap(const RendererMap&) = delete;
        RendererMap& operator=(const RendererMap&) = delete;

        xg::IRenderer* Get(const xg::RenderableDescriptor& renderableDescriptor);
        const xg::IRenderer* Get(const xg::RenderableDescriptor& renderableDescriptor) const;

        const std::unordered_map<xg::RenderableDescriptor, std::unique_ptr<xg::IRenderer>>& GetMap() const { return m_Map; }

        void Register(std::unique_ptr<xg::IRenderer>&& renderer)
        {
            const xg::RenderableDescriptor& renderableDescriptor = renderer->GetRenderableDescriptor();
            m_Map[renderableDescriptor] = std::move(renderer);
        }

    private:
        std::unordered_map<xg::RenderableDescriptor, std::unique_ptr<xg::IRenderer>> m_Map;
    };
}