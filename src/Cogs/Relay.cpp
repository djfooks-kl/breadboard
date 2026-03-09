#include "Relay.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>

#include "IRenderableAdder.h"

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Relay");
    static const xg::RenderableResourceId s_RenderableRelay = xg::RenderableResourceId::Create("Relay");
    static const std::vector<glm::ivec2> s_WireNodes{ glm::ivec2(0, 0), glm::ivec2(1, 0), glm::ivec2(2, 0) };
}

xg::CogResourceId xg::cog::GetRelayResourceId()
{
    return s_ResourceId;
}

xg::CogResourceId xg::cog::Relay::GetResourceId() const
{
    return s_ResourceId;
}

glm::ivec2 xg::cog::Relay::GetSize() const
{
    return glm::ivec2(1, 3);
}

const std::vector<glm::ivec2>& xg::cog::Relay::GetWireNodes() const
{
    return s_WireNodes;
}

void xg::cog::Relay::AddStaticRenderables(
    const glm::ivec2& position,
    const xc::Rotation90 rotation,
    const xg::IRenderableAdder& renderableAdder) const
{
    renderableAdder.Add(s_RenderableRelay, position, rotation);
}