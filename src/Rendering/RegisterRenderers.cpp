#include "Rendering/RegisterRenderers.h"

#include <memory>

#include "CogNodeRenderer.h"
#include "GridIconRenderer.h"
#include "RenderableResourceId.h"
#include "RendererMap.h"
#include "RenderSettings.h"
#include "ShaderProgramMap.h"
#include "SwitchRenderer.h"
#include "WireEndRenderer.h"
#include "WireLineRenderer.h"

namespace
{
    static const xg::ShaderProgramResourceId s_ShaderBatteryIcon = xg::ShaderProgramResourceId::Create("BatteryIcon");
    static const xg::RenderableResourceId s_RenderableBatteryIcon = xg::RenderableResourceId::Create("BatteryIcon");

    static const xg::ShaderProgramResourceId s_ShaderCogNode = xg::ShaderProgramResourceId::Create("CogNode");
    static const xg::RenderableResourceId s_RenderableInputCogNode = xg::RenderableResourceId::Create("InputCogNode");
    static const xg::RenderableResourceId s_RenderableOutputCogNode = xg::RenderableResourceId::Create("OutputCogNode");

    static const xg::ShaderProgramResourceId s_ShaderSwitch = xg::ShaderProgramResourceId::Create("Switch");
    static const xg::RenderableResourceId s_RenderableSwitch = xg::RenderableResourceId::Create("Switch");

    static const xg::ShaderProgramResourceId s_ShaderWireCircle = xg::ShaderProgramResourceId::Create("WireCircle");
    static const xg::RenderableResourceId s_RenderableWireCircleTop = xg::RenderableResourceId::Create("WireCircleTop");
    static const xg::RenderableResourceId s_RenderableWireCircleBottom = xg::RenderableResourceId::Create("WireCircleBottom");

    static const xg::ShaderProgramResourceId s_ShaderWire = xg::ShaderProgramResourceId::Create("Wire");
    static const xg::RenderableResourceId s_RenderableWire = xg::RenderableResourceId::Create("Wire");

    constexpr float s_WireLineHeight = 1.f;
    constexpr float s_WireDotTopHeight = 2.f;

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

void xg::RegisterCogRenderers(
    const xg::RenderSettings& settings,
    xg::CogRendererMap& map,
    xg::ShaderProgramMap& shaderProgramMap,
    xg::ERenderingMode mode)
{
    RegisterShaderProgram(shaderProgramMap, s_ShaderBatteryIcon, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/IconVertex.glsl",
        .m_FragmentPath = "shaders/BatteryIconFragment.glsl" });

    {
        std::unique_ptr<xg::GridIconRenderer> renderer = std::make_unique<xg::GridIconRenderer>(shaderProgramMap.at(s_ShaderBatteryIcon));
        renderer->SetIconSize(1.f);
        if (mode == ERenderingMode::DropPreview)
            renderer->SetColor(settings.m_DropPreviewColor);
        map.Register(s_RenderableBatteryIcon, std::move(renderer));
    }

    RegisterShaderProgram(shaderProgramMap, s_ShaderCogNode, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/CogNodeVertex.glsl",
        .m_FragmentPath = "shaders/CogNodeFragment.glsl" });

    {
        std::unique_ptr<xg::CogNodeRenderer> renderer = std::make_unique<xg::CogNodeRenderer>(shaderProgramMap.at(s_ShaderCogNode));
        renderer->SetRingColor(glm::vec3(0.f, 1.f, 0.f));
        renderer->SetRadius(settings.m_NodeSize);
        renderer->SetInnerRadius(settings.m_NodeInnerRadius);
        renderer->SetOuterRadius(settings.m_NodeOuterRadius);
        if (mode == ERenderingMode::DropPreview)
            renderer->SetRingColor(settings.m_DropPreviewColor);
        map.Register(s_RenderableInputCogNode, std::move(renderer));
    }
    {
        std::unique_ptr<xg::CogNodeRenderer> renderer = std::make_unique<xg::CogNodeRenderer>(shaderProgramMap.at(s_ShaderCogNode));
        renderer->SetRingColor(glm::vec3(0.f, 0.f, 0.f));
        renderer->SetRadius(settings.m_NodeSize);
        renderer->SetInnerRadius(settings.m_NodeInnerRadius);
        renderer->SetOuterRadius(settings.m_NodeOuterRadius);
        if (mode == ERenderingMode::DropPreview)
            renderer->SetRingColor(settings.m_DropPreviewColor);
        map.Register(s_RenderableOutputCogNode, std::move(renderer));
    }

    RegisterShaderProgram(shaderProgramMap, s_ShaderSwitch, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/SwitchVertex.glsl",
        .m_FragmentPath = "shaders/SwitchFragment.glsl" });

    {
        std::unique_ptr<xg::SwitchRenderer> renderer = std::make_unique<xg::SwitchRenderer>(shaderProgramMap.at(s_ShaderSwitch));
        if (mode == ERenderingMode::DropPreview)
            renderer->SetColor(settings.m_DropPreviewColor);
        renderer->SetHasInfoTexture(mode == ERenderingMode::Normal);
        map.Register(s_RenderableSwitch, std::move(renderer));
    }

    map.SortRenderers();
}

void xg::RegisterWireRenderers(
    const xg::RenderSettings& settings,
    xg::WireRendererMap& map,
    xg::ShaderProgramMap& shaderProgramMap,
    xg::ERenderingMode mode)
{
    RegisterShaderProgram(shaderProgramMap, s_ShaderWireCircle, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/WireCircleVertex.glsl",
        .m_FragmentPath = "shaders/WireCircleFragment.glsl" });

    {
        std::unique_ptr<xg::WireEndRenderer> renderer = std::make_unique<xg::WireEndRenderer>(shaderProgramMap.at(s_ShaderWireCircle));
        renderer->SetColorEmpty(glm::vec3(1.f));
        renderer->SetColorFull(settings.m_WireFullColor);
        renderer->SetSize(0.14f);
        renderer->SetHasInfoTexture(mode == ERenderingMode::Normal);
        renderer->SetHeight(s_WireDotTopHeight);
        map.Register(s_RenderableWireCircleTop, std::move(renderer));
    }
    {
        std::unique_ptr<xg::WireEndRenderer> renderer = std::make_unique<xg::WireEndRenderer>(shaderProgramMap.at(s_ShaderWireCircle));
        renderer->SetColorEmpty(glm::vec3(0.f));
        renderer->SetColorFull(glm::vec3(0.f));
        renderer->SetSize(0.19f);
        renderer->SetHasInfoTexture(false);
        map.Register(s_RenderableWireCircleBottom, std::move(renderer));
    }

    RegisterShaderProgram(shaderProgramMap, s_ShaderWire, xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/WireLineVertex.glsl",
        .m_FragmentPath = "shaders/WireLineFragment.glsl" });

    {
        std::unique_ptr<xg::WireLineRenderer> renderer = std::make_unique<xg::WireLineRenderer>(shaderProgramMap.at(s_ShaderWire));
        renderer->m_Uniforms.m_ColorEmpty = glm::vec3(1.f);
        renderer->m_Uniforms.m_ColorFull = settings.m_WireFullColor;
        if (mode == ERenderingMode::DropPreview)
            renderer->m_Uniforms.m_ColorEmpty = settings.m_DropPreviewColor;
        renderer->m_Uniforms.m_HasInfoTexture = mode == ERenderingMode::Normal;
        renderer->SetHeight(s_WireLineHeight);
        map.Register(s_RenderableWire, std::move(renderer));
    }

    map.SortRenderers();
}
