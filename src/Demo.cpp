#include "Demo.h"

#include <array>
#include <iostream>
#include <ImGui/imgui.h>

#include "Engine/ShaderProgram.h"
#include "DemoSystem.h"
#include "DemoColorAnimationComponent.h"
#include "GLFWLib.h"

namespace
{
    constexpr int s_TextureWidth = 2;
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureIndex = 1;

    void SetTextureData(const flecs::world& world)
    {
        std::array<uint8_t, 6> data = {
            0, 0, 255,
            0, 255, 0 };

        auto query = world.query_builder<const demo::ColorAnimationComponent>();
        query.each([&](const demo::ColorAnimationComponent& color) {
            data[0] = color.m_R;
            data[1] = color.m_G;
            data[2] = color.m_B;
        });

        const GLsizei height = 1;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s_TextureWidth, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }
}

Demo::Demo()
    : m_Program(std::make_unique<bread::engine::ShaderProgram>(
        bread::engine::ShaderProgramOptions{ .m_Name = "demo", .m_VertexPath = "vertex.glsl", .m_FragmentPath = "fragment.glsl" }))
{
}

Demo::~Demo() = default;

void Demo::Update(const double time, const float deltaTime)
{
    demo_system::Update(m_World, time, deltaTime);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_Program->GetProgramId());
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(s_AttributePosition);

    glVertexAttribPointer(s_AttributeTextureIndex, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(s_AttributeTextureIndex);

    glBindTexture(GL_TEXTURE_2D, m_Texture);
    SetTextureData(m_World);

    const float animation = static_cast<float>(std::abs(static_cast<uint8_t>(time * 100.f) - 128)) / 200.f;
    GLint transformUniform = glGetUniformLocation(m_Program->GetProgramId(), "transform");
    glUniform2f(transformUniform, 0, animation);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ImGui menu"))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("World");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    const bool clickWindow = ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::GetIO().WantCaptureMouse;
    if (clickWindow || m_ContextMenuOpen)
    {
        m_ContextMenuOpen = ImGui::BeginPopupContextVoid(nullptr, ImGuiPopupFlags_MouseButtonLeft);
        if (m_ContextMenuOpen)
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Popup");
            ImGui::EndPopup();
        }
    }
}

void Demo::Init()
{
    m_Program->TryLoadAndOutputError();

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SetTextureData(m_World);
    glGenerateMipmap(GL_TEXTURE_2D);

    const float fWidth = s_TextureWidth;
    float vertices[] = {
        // positions    texture index
        0.0f,  0.5f,    0.f / fWidth,
       -0.5f, -0.5f,    1.f / fWidth,
        0.5f, -0.5f,    2.f / fWidth
    };

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
