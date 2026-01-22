#include "BreadRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "CameraComponent.h"
#include "CogBoxRenderer.h"
#include "Cogs/CogMap.h"
#include "Core/Font.h"
#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "GridRenderer.h"
#include "TextRenderer.h"
#include "UIDragPreviewComponent.h"
#include "UIPreviewAddingCogComponent.h"

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

    m_TextProgram->TryLoadAndOutputError();
    m_GridProgram->TryLoadAndOutputError();
    m_CogBoxProgram->TryLoadAndOutputError();

    m_Font = std::make_unique<xc::Font>();
    m_Font->Load(DATA_DIR "/sourcecodepro-medium.png", DATA_DIR "/sourcecodepro-medium.json");

    m_TextRenderer = std::make_unique<xg::TextRenderer>(*m_Font, *m_TextProgram);
    m_TextRenderer->AddString(m_Text, m_FontSize, m_Position.x, m_Position.y, m_Color);

    m_GridRenderer = std::make_unique<xg::GridRenderer>(*m_GridProgram);

    m_CogBoxRenderer = std::make_unique<xg::CogBoxRenderer>(*m_CogBoxProgram);
    m_CogBoxRenderer->AddBox(glm::vec2(2, 0), glm::vec2(2, 1));
    m_CogBoxRenderer->AddBox(glm::vec2(5, 0), glm::vec2(6, 0));

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

}

void xg::BreadRenderer::Update(const flecs::world& world)
{
}

void xg::BreadRenderer::Draw(const flecs::world& world)
{
    const auto& camera = world.get<xg::CameraComponent>();
    const auto& cogMap = world.get<xg::CogMap>();

    m_GridRenderer->Draw(camera.m_ViewProjection, camera.m_InvViewProjection, camera.m_Feather);

    m_CogBoxRenderer->Draw(camera.m_ViewProjection, camera.m_Feather);

    const auto& previewAddingCog = world.get<xg::UIPreviewAddingCogComponent>();
    m_CogBoxPreviewDropRenderer->RemoveAllBoxes();
    world.each([&](const xg::UIDragPreviewComponent& dragPreview)
        {
            if (previewAddingCog.m_HoverCogId)
                return;

            const xg::CogPrototype* cog = cogMap.Get(dragPreview.m_CogId);
            glm::ivec2 cogExtents = cog->GetSize() - glm::ivec2(1, 1);
            cogExtents = dragPreview.m_Rotation.GetIMatrix() * cogExtents;

            m_CogBoxPreviewDropRenderer->AddBox(glm::vec2(0, 0), cogExtents);

            const glm::vec2 previewCogPosition = glm::vec2(dragPreview.m_Position) - glm::vec2(cogExtents);

            const glm::vec2 relativeCameraPos = camera.m_Position - previewCogPosition;
            const glm::vec3 cameraPos = glm::vec3(relativeCameraPos, 0.5f);
            const glm::vec3 cameraTarget = glm::vec3(relativeCameraPos, 0.0f);
            const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 previewCameraView = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            m_CogBoxPreviewDropRenderer->Draw(camera.m_Projection * previewCameraView, camera.m_Feather);
        });

    m_CogBoxPreviewRenderer->RemoveAllBoxes();
    world.each([&](const xg::UIDragPreviewComponent& dragPreview)
        {
            const xg::CogPrototype* cog = cogMap.Get(dragPreview.m_CogId);
            glm::ivec2 cogExtents = cog->GetSize() - glm::ivec2(1, 1);
            cogExtents = dragPreview.m_Rotation.GetIMatrix() * cogExtents;

            m_CogBoxPreviewRenderer->AddBox(glm::vec2(0, 0), cogExtents);

            const glm::vec2 previewCogPosition = dragPreview.m_PreviewPosition - glm::vec2(cogExtents);

            const glm::vec2 relativeCameraPos = camera.m_Position - previewCogPosition;
            const glm::vec3 cameraPos = glm::vec3(relativeCameraPos, 0.5f);
            const glm::vec3 cameraTarget = glm::vec3(relativeCameraPos, 0.0f);
            const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 previewCameraView = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            m_CogBoxPreviewRenderer->Draw(camera.m_Projection * previewCameraView, camera.m_Feather);
        });

    m_TextRenderer->Draw(camera.m_ViewProjection);
}
