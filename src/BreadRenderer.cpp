#include "BreadRenderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <array>

#include "CameraComponent.h"
#include "CogBoxRenderer.h"
#include "CogComponent.h"
#include "CogNodeRenderer.h"
#include "Cogs/CogMap.h"
#include "Core/Font.h"
#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "GridAttachmentsComponent.h"
#include "GridHelpers.h"
#include "GridIconRenderer.h"
#include "GridRenderer.h"
#include "GridSizeComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageComponent.h"
#include "OnStageRemovedComponent.h"
#include "Rendering/RegisterRenderers.h"
#include "Rendering/RenderableAdder.h"
#include "RenderSettings.h"
#include "TextRenderer.h"
#include "UIDragPreviewComponent.h"
#include "UIDragValidComponent.h"
#include "UIPreviewAddingCogComponent.h"
#include "UIWireSegmentsComponent.h"
#include "UIWireValidComponent.h"
#include "WireComponent.h"
#include "WireTextureSizeComponent.h"

namespace
{
    static const xg::RenderableResourceId s_RenderableWireCircleTop = xg::RenderableResourceId::Create("WireCircleTop");
    static const xg::RenderableResourceId s_RenderableWireCircleBottom = xg::RenderableResourceId::Create("WireCircleBottom");
    static const xg::RenderableResourceId s_RenderableWire = xg::RenderableResourceId::Create("Wire");
}

xg::BreadRenderer::BreadRenderer()
{
}

xg::BreadRenderer::~BreadRenderer()
{
    m_Font.reset();
    m_TextRenderer.reset();

    m_TextProgram.reset();
    m_GridProgram.reset();
    m_CogBoxProgram.reset();

    glDeleteTextures(1, &m_WireTexture);
}

void xg::BreadRenderer::Load(const xg::RenderSettings& settings)
{
    xg::RegisterCogRenderers(settings, m_CogRendererMap, m_ShaderProgramMap, xg::ERenderingMode::Normal);
    xg::RegisterCogRenderers(settings, m_CogPreviewRendererMap, m_ShaderProgramMap, xg::ERenderingMode::Preview);
    xg::RegisterCogRenderers(settings, m_CogPreviewDropRendererMap, m_ShaderProgramMap, xg::ERenderingMode::DropPreview);

    xg::RegisterWireRenderers(settings, m_WireRendererMap, m_ShaderProgramMap, xg::ERenderingMode::Normal);
    xg::RegisterWireRenderers(settings, m_WirePreviewRendererMap, m_ShaderProgramMap, xg::ERenderingMode::Preview);

    m_TextProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/BoxVertex.glsl",
        .m_FragmentPath = "shaders/TextFragment.glsl" });
    m_TextProgram->TryLoadAndOutputError();

    m_GridProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/GridVertex.glsl",
        .m_FragmentPath = "shaders/GridFragment.glsl" });
    m_GridProgram->TryLoadAndOutputError();

    m_CogBoxProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/CogBoxVertex.glsl",
        .m_FragmentPath = "shaders/CogBoxFragment.glsl" });
    m_CogBoxProgram->TryLoadAndOutputError();

    m_CogNodeProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/CogNodeVertex.glsl",
        .m_FragmentPath = "shaders/CogNodeFragment.glsl" });
    m_CogNodeProgram->TryLoadAndOutputError();

    m_Font = std::make_unique<xc::Font>();
    m_Font->Load(DATA_DIR "/sourcecodepro-medium.png", DATA_DIR "/sourcecodepro-medium.json");

    m_TextRenderer = std::make_unique<xg::TextRenderer>(*m_Font, *m_TextProgram);
    m_TextRenderer->AddString(m_Text, m_FontSize, m_Position.x, m_Position.y, m_Color);

    m_GridRenderer = std::make_unique<xg::GridRenderer>(*m_GridProgram);

    xg::CogBoxUniforms cogBoxDefaultUniforms;
    cogBoxDefaultUniforms.m_Color = glm::vec3(0.f, 0.f, 0.f);
    cogBoxDefaultUniforms.m_FillColor = glm::vec3(1.f, 1.f, 1.f);
    cogBoxDefaultUniforms.m_Size = settings.m_CogBoxSize;
    cogBoxDefaultUniforms.m_Expand = 0.f;

    m_CogBoxRenderer = std::make_unique<xg::CogBoxRenderer>(*m_CogBoxProgram);
    m_CogBoxRenderer->m_Uniforms = cogBoxDefaultUniforms;

    m_CogBoxPreviewRenderer = std::make_unique<xg::CogBoxRenderer>(*m_CogBoxProgram);
    m_CogBoxPreviewRenderer->m_Uniforms = cogBoxDefaultUniforms;

    m_CogBoxPreviewDropRenderer = std::make_unique<xg::CogBoxRenderer>(*m_CogBoxProgram);
    m_CogBoxPreviewDropRenderer->m_Uniforms = cogBoxDefaultUniforms;
    m_CogBoxPreviewDropRenderer->m_Uniforms.m_Color = glm::vec3(0.5f, 0.5f, 0.5f);

    m_CogNodeRenderer = std::make_unique<xg::CogNodeRenderer>(*m_CogNodeProgram);
    m_CogNodeRenderer->m_Uniforms.m_RingColor = glm::vec3(0.f, 1.f, 0.f);
    m_CogNodeRenderer->m_Uniforms.m_InnerRadius = settings.m_NodeInnerRadius;
    m_CogNodeRenderer->m_Uniforms.m_OuterRadius = settings.m_NodeOuterRadius;
    m_CogNodeRenderer->AddNode(glm::ivec2(1, 1), glm::ivec2(0, 0));
    m_CogNodeRenderer->AddNode(glm::ivec2(2, 1), glm::ivec2(1, 0));
    m_CogNodeRenderer->AddNode(glm::ivec2(3, 1), glm::ivec2(2, 0));
    m_CogNodeRenderer->AddNode(glm::ivec2(4, 1), glm::ivec2(3, 0));
    m_CogNodeRenderer->AddNode(glm::ivec2(5, 1), glm::ivec2(4, 0));
    m_CogNodeRenderer->AddNode(glm::ivec2(6, 1), glm::ivec2(5, 0));
    m_CogNodeRenderer->AddNode(glm::ivec2(7, 1), glm::ivec2(6, 0));
    m_CogNodeRenderer->AddNode(glm::ivec2(8, 1), glm::ivec2(7, 0));
}

void xg::BreadRenderer::Update(const flecs::world& world)
{
    const glm::ivec2& wireTextureSize = world.get<xg::WireTextureSizeComponent>().m_Size;
    if (wireTextureSize != m_WireTextureSize)
    {
        m_WireTextureSize = wireTextureSize;

        glDeleteTextures(1, &m_WireTexture);
        glGenTextures(1, &m_WireTexture);
        glBindTexture(GL_TEXTURE_2D, m_WireTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        std::vector<uint8_t> data = { 255, 0, 0, 255, 0, 0, 255, 255 };
        data.resize(wireTextureSize.x * wireTextureSize.y);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, wireTextureSize.x, wireTextureSize.y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    const bool anyOnStageChanges =
        world.count<const xg::OnStageAddedComponent>() != 0 ||
        world.count<const xg::OnStageRemovedComponent>() != 0;

    if (anyOnStageChanges)
    {
        if (world.query<xg::OnStageAddedComponent, xg::CogComponent>().is_true() ||
            world.query<xg::OnStageRemovedComponent, xg::CogComponent>().is_true())
        {
            m_CogBoxRenderer->RemoveAll();
            for (xg::IRenderer* renderer : m_CogRendererMap.GetOrder())
            {
                renderer->RemoveAll();
            }

            const auto& cogMap = world.get<xg::CogMap>();
            world.each([&](
                const xg::OnStageComponent,
                const xg::CogComponent& cogComponent)
            {
                const xg::CogPrototype* cog = cogMap.Get(cogComponent.m_CogId);
                glm::ivec2 cogExtents = cog->GetSize() - glm::ivec2(1, 1);
                m_CogBoxRenderer->AddBox(cogComponent.m_Transform.m_Translation, cogComponent.m_Transform.Apply(cogExtents));

                xg::RenderableAdder renderableAdder("Cog", m_CogRendererMap);
                cog->AddStaticRenderables(cogComponent.m_Transform, renderableAdder);
            });
        }

        if (world.query<xg::OnStageAddedComponent, xg::WireComponent>().is_true() ||
            world.query<xg::OnStageRemovedComponent, xg::WireComponent>().is_true())
        {
            for (xg::IWireRenderer* renderer : m_WireRendererMap.GetOrder())
            {
                renderer->RemoveAll();
            }

            std::unordered_set<glm::ivec2> dots;
            world.each([&](const xg::OnStageComponent, const xg::WireComponent& wire)
            {
                glm::ivec2 prev = wire.m_Checkpoints[0];
                for (int i = 1; i < wire.m_Checkpoints.size(); ++i)
                {
                    const glm::ivec2& current = wire.m_Checkpoints[i];
                    m_WireRendererMap.Get(s_RenderableWire)->AddWire(prev, current, glm::ivec2(0, 0));
                    prev = current;
                }
            });
        }

        const auto& attachments = world.get<xg::GridAttachmentsComponent>().m_Map;

        std::unordered_set<glm::ivec2> dots;
        world.each([&](const xg::OnStageComponent, const xg::WireComponent& wire)
        {
            xg::ForEachSegmentsCellUntil(wire.m_Checkpoints, [&](const glm::ivec2& p)
            {
                auto itr = attachments.find(p);
                if (itr != attachments.end() && itr->second.m_HasWireDot)
                {
                    dots.insert(p);
                }
            });
        });

        m_WireRendererMap.Get(s_RenderableWireCircleTop)->RemoveAll();
        m_WireRendererMap.Get(s_RenderableWireCircleBottom)->RemoveAll();
        for (const glm::ivec2& p : dots)
        {
            m_WireRendererMap.Get(s_RenderableWireCircleTop)->AddWireEnd(p, glm::ivec2(0, 0));
            m_WireRendererMap.Get(s_RenderableWireCircleBottom)->AddWireEnd(p, glm::ivec2(0, 0));
        }
    }
}

void xg::BreadRenderer::Draw(const flecs::world& world)
{
    const auto& camera = world.get<xg::CameraComponent>();
    const auto& cogMap = world.get<xg::CogMap>();
    const glm::ivec2& gridSize = world.get<xg::GridSizeComponent>().m_Size;
    const glm::ivec2& wireTextureSize = world.get<xg::WireTextureSizeComponent>().m_Size;

    m_GridRenderer->Draw(camera.m_ViewProjection, camera.m_InvViewProjection, gridSize, camera.m_Feather);

    m_CogBoxRenderer->Draw(camera.m_ViewProjection, camera.m_Feather);
    for (xg::IRenderer* renderer : m_CogRendererMap.GetOrder())
    {
        renderer->Draw(camera.m_ViewProjection, camera.m_Feather, wireTextureSize, m_WireTexture);
    }
    for (xg::IWireRenderer* renderer : m_WireRendererMap.GetOrder())
    {
        renderer->Draw(camera.m_ViewProjection, camera.m_Feather, wireTextureSize, m_WireTexture);
    }

    const auto& previewAddingCog = world.get<xg::UIPreviewAddingCogComponent>();
    const bool dragValid = world.get<xg::UIDragValidComponent>().m_Valid;
    m_CogBoxPreviewDropRenderer->RemoveAll();
    for (xg::IRenderer* renderer : m_CogPreviewDropRendererMap.GetOrder())
    {
        renderer->RemoveAll();
    }
    if (dragValid)
    {
        world.each([&](const xg::UIDragPreviewComponent& dragPreview)
            {
                if (previewAddingCog.m_HoverCogId)
                    return;

                const xg::CogPrototype* cog = cogMap.Get(dragPreview.m_CogId);
                glm::ivec2 cogExtents = cog->GetSize() - glm::ivec2(1, 1);
                cogExtents = dragPreview.m_Rotation.GetIMatrix() * cogExtents;

                m_CogBoxPreviewDropRenderer->AddBox(glm::vec2(0, 0), cogExtents);

                const glm::vec2 previewCogPosition = glm::vec2(dragPreview.m_Position);

                const glm::vec2 relativeCameraPos = camera.m_Position - previewCogPosition;
                const glm::vec3 cameraPos = glm::vec3(relativeCameraPos, 0.5f);
                const glm::vec3 cameraTarget = glm::vec3(relativeCameraPos, 0.0f);
                const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

                const glm::mat4 previewCameraView = glm::lookAt(cameraPos, cameraTarget, cameraUp);
                const glm::mat4 previewViewProjection = camera.m_Projection * previewCameraView;
                m_CogBoxPreviewDropRenderer->Draw(previewViewProjection, camera.m_Feather);

                xg::RenderableAdder renderableAdder("CogPreview", m_CogPreviewDropRendererMap);
                cog->AddStaticRenderables({ glm::ivec2(0, 0), dragPreview.m_Rotation }, renderableAdder);

                for (xg::IRenderer* renderer : m_CogPreviewDropRendererMap.GetOrder())
                {
                    renderer->Draw(previewViewProjection, camera.m_Feather, wireTextureSize, m_WireTexture);
                }
            });
    }

    m_CogBoxPreviewRenderer->m_Uniforms.m_Color = glm::vec3(dragValid ? 0.f : 1.f, 0.f, 0.f);
    m_CogBoxPreviewRenderer->RemoveAll();

    for (xg::IRenderer* renderer : m_CogPreviewRendererMap.GetOrder())
    {
        renderer->RemoveAll();
    }
    world.each([&](const xg::UIDragPreviewComponent& dragPreview)
        {
            const xg::CogPrototype* cog = cogMap.Get(dragPreview.m_CogId);
            glm::ivec2 cogExtents = cog->GetSize() - glm::ivec2(1, 1);
            cogExtents = dragPreview.m_Rotation.GetIMatrix() * cogExtents;

            m_CogBoxPreviewRenderer->AddBox(glm::ivec2(0, 0), cogExtents);

            glm::vec2 offset(0.f, 0.f);
            if (world.get<xg::UIPreviewAddingCogComponent>().m_HoverCogId)
            {
                offset = -glm::vec2(cogExtents);
            }

            const glm::vec2 previewCogPosition = dragPreview.m_PreviewPosition + offset;

            const glm::vec2 relativeCameraPos = camera.m_Position - previewCogPosition;
            const glm::vec3 cameraPos = glm::vec3(relativeCameraPos, 0.5f);
            const glm::vec3 cameraTarget = glm::vec3(relativeCameraPos, 0.0f);
            const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            const glm::mat4 previewCameraView = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            const glm::mat4 previewViewProjection = camera.m_Projection * previewCameraView;
            m_CogBoxPreviewRenderer->Draw(previewViewProjection, camera.m_Feather);

            xg::RenderableAdder renderableAdder("CogPreview", m_CogPreviewRendererMap);
            cog->AddStaticRenderables({ glm::ivec2(0, 0), dragPreview.m_Rotation }, renderableAdder);

            for (xg::IRenderer* renderer : m_CogPreviewRendererMap.GetOrder())
            {
                renderer->Draw(previewViewProjection, camera.m_Feather, wireTextureSize, m_WireTexture);
            }
        });

    m_CogNodeRenderer->Draw(camera.m_ViewProjection, camera.m_Feather, wireTextureSize, m_WireTexture);

    for (xg::IWireRenderer* renderer : m_WirePreviewRendererMap.GetOrder())
    {
        renderer->RemoveAll();
    }
    bool allPreviewWiresValid = true;
    world.each([&](const xg::UIWireSegmentsComponent& wire, const xg::UIWireValidComponent& validComponent)
        {
            for (const glm::ivec2& checkpoint : wire.m_Checkpoints)
            {
                m_WirePreviewRendererMap.Get(s_RenderableWireCircleTop)->AddWireEnd(checkpoint, glm::ivec2(0, 0));
                m_WirePreviewRendererMap.Get(s_RenderableWireCircleBottom)->AddWireEnd(checkpoint, glm::ivec2(0, 0));
            }
            if (wire.m_Checkpoints.empty())
                return;
            glm::ivec2 prev = wire.m_Checkpoints[0];
            for (int i = 1; i < wire.m_Checkpoints.size(); ++i)
            {
                const glm::ivec2& current = wire.m_Checkpoints[i];
                m_WirePreviewRendererMap.Get(s_RenderableWire)->AddWire(prev, current, glm::ivec2(0, 0));
                prev = current;
            }

            allPreviewWiresValid = allPreviewWiresValid && validComponent.m_Valid;
        });
    for (xg::IWireRenderer* renderer : m_WirePreviewRendererMap.GetOrder())
    {
        renderer->SetValid(allPreviewWiresValid);
        renderer->Draw(camera.m_ViewProjection, camera.m_Feather, wireTextureSize, m_WireTexture);
    }

    m_TextRenderer->Draw(camera.m_ViewProjection);
}
