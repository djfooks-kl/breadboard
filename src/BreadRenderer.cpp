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
#include "GridRenderer.h"
#include "GridSizeComponent.h"
#include "OnStageAddedComponent.h"
#include "OnStageComponent.h"
#include "OnStageRemovedComponent.h"
#include "TextRenderer.h"
#include "UIDragPreviewComponent.h"
#include "UIDragValidComponent.h"
#include "UIPreviewAddingCogComponent.h"
#include "WireTextureSizeComponent.h"

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

void xg::BreadRenderer::Load()
{
    m_TextProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/BoxVertex.glsl",
        .m_FragmentPath = "shaders/TextFragment.glsl" });

    m_GridProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/GridVertex.glsl",
        .m_FragmentPath = "shaders/GridFragment.glsl" });

    m_CogBoxProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/CogBoxVertex.glsl",
        .m_FragmentPath = "shaders/CogBoxFragment.glsl" });

    m_CogNodeProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/CogNodeVertex.glsl",
        .m_FragmentPath = "shaders/CogNodeFragment.glsl" });

    m_TextProgram->TryLoadAndOutputError();
    m_GridProgram->TryLoadAndOutputError();
    m_CogBoxProgram->TryLoadAndOutputError();
    m_CogNodeProgram->TryLoadAndOutputError();

    m_Font = std::make_unique<xc::Font>();
    m_Font->Load(DATA_DIR "/sourcecodepro-medium.png", DATA_DIR "/sourcecodepro-medium.json");

    m_TextRenderer = std::make_unique<xg::TextRenderer>(*m_Font, *m_TextProgram);
    m_TextRenderer->AddString(m_Text, m_FontSize, m_Position.x, m_Position.y, m_Color);

    m_GridRenderer = std::make_unique<xg::GridRenderer>(*m_GridProgram);

    m_CogBoxRenderer = std::make_unique<xg::CogBoxRenderer>(*m_CogBoxProgram);
    m_CogBoxRenderer->SetColor(glm::vec3(0.f, 0.f, 0.f));
    m_CogBoxRenderer->SetFillColor(glm::vec3(1.f, 1.f, 1.f));
    m_CogBoxRenderer->m_Border = 0.4f;
    m_CogBoxRenderer->m_Expand = 0.f;

    m_CogBoxPreviewRenderer = std::make_unique<xg::CogBoxRenderer>(*m_CogBoxProgram);
    m_CogBoxPreviewRenderer->SetColor(glm::vec3(0.f, 0.f, 0.f));
    m_CogBoxPreviewRenderer->SetFillColor(glm::vec3(1.f, 1.f, 1.f));
    m_CogBoxPreviewRenderer->m_Border = 0.4f;
    m_CogBoxPreviewRenderer->m_Expand = 0.f;

    m_CogBoxPreviewDropRenderer = std::make_unique<xg::CogBoxRenderer>(*m_CogBoxProgram);
    m_CogBoxPreviewDropRenderer->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
    m_CogBoxPreviewDropRenderer->SetFillColor(glm::vec3(1.f, 1.f, 1.f));
    m_CogBoxPreviewDropRenderer->m_Border = 0.4f;
    m_CogBoxPreviewDropRenderer->m_Expand = 0.f;

    m_CogNodeRenderer = std::make_unique<xg::CogNodeRenderer>(*m_CogNodeProgram);
    m_CogNodeRenderer->SetRingColor(glm::vec3(0.f, 1.f, 0.f));
    m_CogNodeRenderer->SetRadius(0.7f);
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
    const bool anyOnStageChanges =
        !world.get<const xg::OnStageAddedComponent>().m_Entities.empty() ||
        !world.get<const xg::OnStageRemovedComponent>().m_Entities.empty();

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

    if (anyOnStageChanges)
    {
        m_CogBoxRenderer->RemoveAllBoxes();

        const auto& cogMap = world.get<xg::CogMap>();
        world.each([&](
            const xg::OnStageComponent,
            const xg::CogComponent& cogComponent)
        {
            const xg::CogPrototype* cog = cogMap.Get(cogComponent.m_CogId);
            glm::ivec2 cogExtents = cog->GetSize() - glm::ivec2(1, 1);
            cogExtents = cogComponent.m_Rotation.GetIMatrix() * cogExtents;

            m_CogBoxRenderer->AddBox(cogComponent.m_Position, cogComponent.m_Position + cogExtents);
        });
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

    const auto& previewAddingCog = world.get<xg::UIPreviewAddingCogComponent>();
    const bool dragValid = world.get<xg::UIDragValidComponent>().m_Valid;
    m_CogBoxPreviewDropRenderer->RemoveAllBoxes();
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
                m_CogBoxPreviewDropRenderer->Draw(camera.m_Projection * previewCameraView, camera.m_Feather);
            });
    }

    m_CogBoxPreviewRenderer->SetColor(glm::vec3(dragValid ? 0.f : 1.f, 0.f, 0.f));
    m_CogBoxPreviewRenderer->RemoveAllBoxes();
    world.each([&](const xg::UIDragPreviewComponent& dragPreview)
        {
            const xg::CogPrototype* cog = cogMap.Get(dragPreview.m_CogId);
            glm::ivec2 cogExtents = cog->GetSize() - glm::ivec2(1, 1);
            cogExtents = dragPreview.m_Rotation.GetIMatrix() * cogExtents;

            m_CogBoxPreviewRenderer->AddBox(glm::vec2(0, 0), cogExtents);

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
            m_CogBoxPreviewRenderer->Draw(camera.m_Projection * previewCameraView, camera.m_Feather);
        });

    m_CogNodeRenderer->Draw(camera.m_ViewProjection, camera.m_Feather, wireTextureSize, m_WireTexture);

    m_TextRenderer->Draw(camera.m_ViewProjection);
}
