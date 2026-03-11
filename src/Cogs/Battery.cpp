#include "Battery.h"

#include <glm/ext/vector_int2.hpp>
#include <flecs/flecs.h>
#include "IRenderableAdder.h"

namespace
{
    static const xg::CogResourceId s_ResourceId = xg::CogResourceId::Create("Battery");
    static const xg::RenderableResourceId s_RenderableBatteryIcon = xg::RenderableResourceId::Create("BatteryIcon");
    static const xg::RenderableResourceId s_RenderableOutputCogNode = xg::RenderableResourceId::Create("OutputCogNode");
    static const std::vector<glm::ivec2> s_WireNodes{ glm::ivec2(0, 0) };
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

const std::vector<glm::ivec2>& xg::cog::Battery::GetWireNodes() const
{
    return s_WireNodes;
}

void xg::cog::Battery::AddStaticRenderables(
    const glm::ivec2& position,
    const xc::Rotation90 rotation,
    const xg::IRenderableAdder& renderableAdder) const
{
    renderableAdder.Add(s_RenderableBatteryIcon, position, rotation);
    renderableAdder.Add(s_RenderableOutputCogNode, position + rotation.GetIMatrix() * glm::ivec2(0, 1), rotation);
}
