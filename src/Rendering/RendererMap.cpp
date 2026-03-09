#include "Rendering/RendererMap.h"

xg::IRenderer* xg::RendererMap::Get(const xg::RenderableDescriptor& renderableDescriptor)
{
    auto itr = m_Map.find(renderableDescriptor);
    if (itr != m_Map.end())
    {
        return itr->second.get();
    }
    return nullptr;
}

const xg::IRenderer* xg::RendererMap::Get(const xg::RenderableDescriptor& renderableDescriptor) const
{
    auto itr = m_Map.find(renderableDescriptor);
    if (itr != m_Map.end())
    {
        return itr->second.get();
    }
    return nullptr;
}