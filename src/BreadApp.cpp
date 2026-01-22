#include "BreadApp.h"

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BoxRenderer.h"
#include "BreadEntityWorld.h"
#include "BreadRenderer.h"
#include "CameraComponent.h"
#include "Cogs/CogMap.h"
#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "InputSystem.h"
#include "MouseTrailComponent.h"
#include "UI.h"
#include "UIPreviewAddingCogComponent.h"
#include "WindowSizeSystem.h"

namespace
{
    constexpr int s_TextureWidth = 2;
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureIndex = 1;
    constexpr float s_BlueSpeed = 100.f;
    constexpr float s_GreenSpeed = 150.f;

    // TODO move to BaseApp?
    void WindowSizeCallback(GLFWwindow* /*window*/, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void SetTextureData(const double time)
    {
        std::array<uint8_t, 6> data = {
            255, 0, static_cast<uint8_t>(time * s_BlueSpeed),
            255, static_cast<uint8_t>(255.f - time * s_GreenSpeed), 0 };

        const GLsizei height = 1;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s_TextureWidth, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }
}

BreadApp::BreadApp()
{
}

BreadApp::~BreadApp()
{
    m_DemoProgram.reset();
    m_BoxProgram.reset();

    glDeleteVertexArrays(1, &m_DemoVBO);
    glDeleteBuffers(1, &m_PositionsBuffer);
    glDeleteBuffers(1, &m_TextureUBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
    glDeleteTextures(1, &m_Texture);
}

void BreadApp::ProcessKeyInput(GLFWwindow* /*window*/, int key, int scancode, int action, int mods)
{
    xg::InputSystem::UpdateKeyInput(m_World, key, scancode, action, mods);
}

void BreadApp::ProcessCursorInput(GLFWwindow* /*window*/, double xpos, double ypos)
{
    xg::InputSystem::UpdateCursorInput(m_World, xpos, ypos);
}

void BreadApp::Render(double time, float /*deltaTime*/)
{
    const auto& camera = m_World.get<xg::CameraComponent>();

    glClear(GL_COLOR_BUFFER_BIT);

    m_BreadRenderer->Draw(m_World);

    glUseProgram(m_DemoProgram->GetProgramId());
    glBindVertexArray(m_DemoVBO);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    SetTextureData(time);

    GLint viewProjectionUniform = glGetUniformLocation(m_DemoProgram->GetProgramId(), "viewProjection");
    glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, glm::value_ptr(camera.m_ViewProjection));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    m_BoxRenderer->RemoveAllBoxes();
    m_World.query<const xg::MouseTrailComponent>()
        .each([&](
            const xg::MouseTrailComponent& mouseTrail)
    {
        for (const xg::TrailPoint& p : mouseTrail.m_Positions)
        {
            m_BoxRenderer->AddBox(0.05f, p.m_Position, p.m_Color);
        }
    });
    m_BoxRenderer->Draw(camera.m_ViewProjection);
}

void BreadApp::Update(GLFWwindow* window, const double time, const float deltaTime)
{
    xg::WindowSizeSystem::Update(m_World, window);

    xg::UpdateWorld(m_World, time, deltaTime);

    m_BreadRenderer->Update(m_World);

    Render(time, deltaTime);
    m_UI->Draw(m_World);

    // update input system last as it clears all the input state ready for next frame
    xg::InputSystem::Update(m_World);
}

void BreadApp::Init(GLFWwindow* window)
{
    xg::SetupWorld(m_World);

    glfwSetWindowSizeCallback(window, WindowSizeCallback);

    m_BreadRenderer = std::make_unique<xg::BreadRenderer>();
    m_BreadRenderer->Load();

    m_DemoProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/DemoVertex.glsl",
        .m_FragmentPath = "shaders/DemoFragment.glsl" });

    m_BoxProgram = std::make_unique<xc::ShaderProgram>(xc::ShaderProgramOptions{
        .m_VertexPath = "shaders/BoxVertex.glsl",
        .m_FragmentPath = "shaders/BoxFragment.glsl" });

    m_DemoProgram->TryLoadAndOutputError();
    m_BoxProgram->TryLoadAndOutputError();

    m_BoxRenderer = std::make_unique<BoxRenderer>(*m_BoxProgram);

    m_UI = std::make_unique<xg::UI>();

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SetTextureData(0.0);
    glGenerateMipmap(GL_TEXTURE_2D);

    const float size = 0.2f;
    float positions[] = {
       -size, -size,
        size, -size,
       -size,  size,
        size,  size,
    };

    const float fWidth = s_TextureWidth;
    float textureU[] = {
        1.f / fWidth,
        0.f / fWidth,
        2.f / fWidth,
        1.f / fWidth
    };

    //   2---3
    //   | \ |
    //   0---1
    // anti-clockwise winding
    unsigned int indices[] = {
        0,1,2,2,1,3
    };

    glGenBuffers(1, &m_PositionsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glGenBuffers(1, &m_TextureUBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_TextureUBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureU), textureU, GL_STATIC_DRAW);

    glGenBuffers(1, &m_IndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_DemoVBO);
    {
        glBindVertexArray(m_DemoVBO);

        glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
        glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributePosition);

        glBindBuffer(GL_ARRAY_BUFFER, m_TextureUBuffer);
        glVertexAttribPointer(s_AttributeTextureIndex, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributeTextureIndex);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

        glBindVertexArray(0);
    }
}
