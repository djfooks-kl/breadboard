#include "Switch.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>

#include "IRenderableAdder.h"

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Switch");
    static const xg::RenderableResourceId s_RenderableSwitch = xg::RenderableResourceId::Create("Switch");
    static const std::vector<glm::ivec2> s_WireNodes{ glm::ivec2(0, 0), glm::ivec2(0, 1) };
}

xg::CogResourceId xg::cog::GetSwitchResourceId()
{
    return s_ResourceId;
}

xg::CogResourceId xg::cog::Switch::GetResourceId() const
{
    return s_ResourceId;
}

glm::ivec2 xg::cog::Switch::GetSize() const
{
    return glm::ivec2(1, 2);
}

const std::vector<glm::ivec2>& xg::cog::Switch::GetWireNodes() const
{
    return s_WireNodes;
}

void xg::cog::Switch::AddStaticRenderables(
    const xc::ITransform& transform,
    const xg::IRenderableAdder& renderableAdder) const
{
    renderableAdder.Add(s_RenderableSwitch, transform);
}
