#include "Battery.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>
#include "IRenderableAdder.h"

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Battery");
    static const xg::RenderableResourceId s_RenderableBatteryIcon = xg::RenderableResourceId::Create("BatteryIcon");
    static const xg::RenderableResourceId s_RenderableOutputCogNode = xg::RenderableResourceId::Create("OutputCogNode");
    static const std::unordered_set<glm::ivec2> s_WireNodes{ glm::ivec2(0, 1) };
}

xg::CogResourceId xg::cog::GetBatteryResourceId()
{
    return s_ResourceId;
}

xg::CogResourceId xg::cog::Battery::GetResourceId() const
{
    return s_ResourceId;
}

glm::ivec2 xg::cog::Battery::GetSize() const
{
    return glm::ivec2(1, 2);
}

const std::unordered_set<glm::ivec2>& xg::cog::Battery::GetWireNodes() const
{
    return s_WireNodes;
}

void xg::cog::Battery::AddStaticRenderables(
    const xc::ITransform& transform,
    const xg::IRenderableAdder& renderableAdder) const
{
    renderableAdder.Add(s_RenderableBatteryIcon, transform);
    renderableAdder.Add(s_RenderableOutputCogNode, transform.WithLocalTranslation(glm::ivec2(0, 1)));
}
