#include "Relay.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>

#include "IRenderableAdder.h"
#include "Rendering/RenderingFlags.h"

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Relay");
    static const xg::RenderableResourceId s_RenderableSwitch = xg::RenderableResourceId::Create("Switch");
    static const xg::RenderableResourceId s_RenderableOutputCogNode = xg::RenderableResourceId::Create("OutputCogNode");
    static const std::unordered_set<glm::ivec2> s_WireNodes{ glm::ivec2(0, 0), glm::ivec2(1, 0), glm::ivec2(2, 0) };
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
    return glm::ivec2(1, 4);
}

const std::unordered_set<glm::ivec2>& xg::cog::Relay::GetWireNodes() const
{
    return s_WireNodes;
}

void xg::cog::Relay::AddStaticRenderables(
    const xc::ITransform& transform,
    const xg::IRenderableAdder& renderableAdder) const
{
    renderableAdder.Add(s_RenderableSwitch, transform.WithLocalTranslation(glm::ivec2(0, 1)), xg::rendering::s_SwitchDouble);
    renderableAdder.Add(s_RenderableOutputCogNode, transform);
}