#include "Rendering/RendererMap.h"

xg::IRenderer* xg::RendererMap::Get(const xg::RenderableResourceId& renderableResourceId)
{
    auto itr = m_Map.find(renderableResourceId);
    if (itr != m_Map.end())
    {
        return itr->second.get();
    }
    return nullptr;
}

const xg::IRenderer* xg::RendererMap::Get(const xg::RenderableResourceId& renderableResourceId) const
{
    auto itr = m_Map.find(renderableResourceId);
    if (itr != m_Map.end())
    {
        return itr->second.get();
    }
    return nullptr;
}