#include "Rendering/RegisterRenderers.h"

#include <memory>

#include "ShaderProgramMap.h"
#include "RenderableResourceId.h"
#include "RendererMap.h"
#include "GridIconRenderer.h"
#include "CogNodeRenderer.h"
#include "SwitchRenderer.h"

namespace
{
    static const xg::ShaderProgramResourceId s_ShaderBatteryIcon = xg::ShaderProgramResourceId::Create("BatteryIcon");
    static const xg::RenderableResourceId s_RenderableBatteryIcon = xg::RenderableResourceId::Create("BatteryIcon");

    static const xg::ShaderProgramResourceId s_ShaderCogNode = xg::ShaderProgramResourceId::Create("CogNode");
    static const xg::RenderableResourceId s_RenderableInputCogNode = xg::RenderableResourceId::Create("InputCogNode");
    static const xg::RenderableResourceId s_RenderableOutputCogNode = xg::RenderableResourceId::Create("OutputCogNode");

    static const xg::ShaderProgramResourceId s_ShaderSwitch = xg::ShaderProgramResourceId::Create("Switch");
    static const xg::RenderableResourceId s_RenderableSwitch = xg::RenderableResourceId::Create("Switch");

    void RegisterShaderProgram(
        xg::ShaderProgramMap& inout_shaderProgramMap,
        xg::ShaderProgramResourceId shaderProgramId,
        xc::ShaderProgramOptions options)
    {
        auto emplace = inout_shaderProgramMap.try_emplace(shaderProgramId, std::move(options));
        if (emplace.second)
        {
            xc::ShaderProgram& shaderProgram = emplace.first->second;
            shaderProgram.TryLoadAndOutputError();
        }
    }
}

void xg::RegisterCogRenderers(xg::RendererMap& map, xg::ShaderProgramMap& shaderProgramMap, xg::ERenderingMode mode)
{
    RegisterShaderProgram(shaderProgramMap, s_ShaderBatteryIcon, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/IconVertex.glsl",
        .m_FragmentPath = "shaders/BatteryIconFragment.glsl" });

    {
        std::unique_ptr<xg::GridIconRenderer> renderer = std::make_unique<xg::GridIconRenderer>(shaderProgramMap.at(s_ShaderBatteryIcon));
        renderer->SetIconSize(1.f);
        if (mode == ERenderingMode::DropPreview)
            renderer->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
        map.Register(s_RenderableBatteryIcon, std::move(renderer));
    }

    RegisterShaderProgram(shaderProgramMap, s_ShaderCogNode, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/CogNodeVertex.glsl",
        .m_FragmentPath = "shaders/CogNodeFragment.glsl" });

    {
        std::unique_ptr<xg::CogNodeRenderer> renderer = std::make_unique<xg::CogNodeRenderer>(shaderProgramMap.at(s_ShaderCogNode));
        renderer->SetRingColor(glm::vec3(0.f, 1.f, 0.f));
        renderer->SetRadius(0.7f);
        if (mode == ERenderingMode::DropPreview)
            renderer->SetRingColor(glm::vec3(0.5f, 0.5f, 0.5f));
        map.Register(s_RenderableInputCogNode, std::move(renderer));
    }
    {
        std::unique_ptr<xg::CogNodeRenderer> renderer = std::make_unique<xg::CogNodeRenderer>(shaderProgramMap.at(s_ShaderCogNode));
        renderer->SetRingColor(glm::vec3(0.f, 0.f, 0.f));
        renderer->SetRadius(0.7f);
        if (mode == ERenderingMode::DropPreview)
            renderer->SetRingColor(glm::vec3(0.5f, 0.5f, 0.5f));
        map.Register(s_RenderableOutputCogNode, std::move(renderer));
    }

    RegisterShaderProgram(shaderProgramMap, s_ShaderSwitch, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/SwitchVertex.glsl",
        .m_FragmentPath = "shaders/SwitchFragment.glsl" });

    {
        std::unique_ptr<xg::SwitchRenderer> renderer = std::make_unique<xg::SwitchRenderer>(shaderProgramMap.at(s_ShaderSwitch));
        if (mode == ERenderingMode::DropPreview)
        {
            renderer->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
        }
        renderer->SetHasInfoTexture(mode == ERenderingMode::Normal);
        map.Register(s_RenderableSwitch, std::move(renderer));
    }
}
