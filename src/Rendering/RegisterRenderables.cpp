#include "Rendering/RegisterRenderables.h"

#include <memory>

#include "ShaderProgramMap.h"
#include "RenderableResourceId.h"
#include "RendererMap.h"
#include "GridIconRenderer.h"

namespace
{
    static const xg::ShaderProgramResourceId s_ShaderBatteryIcon = xg::ShaderProgramResourceId::Create("BatteryIcon");

    static const xg::RenderableResourceId s_RenderableBatteryIcon = xg::RenderableResourceId::Create("BatteryIcon");

    void RegisterShaderProgram(
        xg::ShaderProgramMap& inout_shaderProgramMap,
        xg::ShaderProgramResourceId shaderProgramId,
        xc::ShaderProgramOptions options)
    {
        xc::ShaderProgram& shaderProgram = inout_shaderProgramMap.try_emplace(shaderProgramId, std::move(options)).first->second;
        shaderProgram.TryLoadAndOutputError();
    }
}

void xg::RegisterCogRenderers(xg::RendererMap& map, xg::ShaderProgramMap& shaderProgramMap)
{
    RegisterShaderProgram(shaderProgramMap, s_ShaderBatteryIcon, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/IconVertex.glsl",
        .m_FragmentPath = "shaders/BatteryIconFragment.glsl" });

    {
        std::unique_ptr<xg::GridIconRenderer> renderer = std::make_unique<xg::GridIconRenderer>(
            xg::RenderableDescriptor{ s_RenderableBatteryIcon },
            shaderProgramMap.at(s_ShaderBatteryIcon));
        renderer->SetIconSize(1.f);
        map.Register(std::move(renderer));
    }
    {
        std::unique_ptr<xg::GridIconRenderer> renderer = std::make_unique<xg::GridIconRenderer>(
            xg::RenderableDescriptor{ s_RenderableBatteryIcon, xg::ERenderableMode::Preview },
            shaderProgramMap.at(s_ShaderBatteryIcon));
        renderer->SetIconSize(1.f);
        renderer->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
        map.Register(std::move(renderer));
    }
}
