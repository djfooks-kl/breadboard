#include "GridIconRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"

namespace
{
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeUV = 1;
    constexpr GLuint s_AttributeTextureUV = 2;
    constexpr GLuint s_AttributeColor = 3;
}

xg::GridIconRenderer::GridIconRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = program.GetUniformLocation("viewProjection");
    m_FeatherUniform = program.GetUniformLocation("feather");
    m_IconSizeUniform = program.GetUniformLocation("iconSize");
    m_ColorUniform = program.GetUniformLocation("color");

    m_VBO.AddIVertexAttribute(s_AttributePosition, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeTextureUV, 2);
    m_VBO.AddFVertexAttribute(s_AttributeColor, 3);
}

void xg::GridIconRenderer::AddRenderable(
    const glm::ivec2& position,
    const xc::Rotation90 rotation,
    const int /*flags*/,
    const glm::ivec2&)
{
    AddIcon(position, rotation, glm::vec3(1.f, 1.f, 1.f));
}

void xg::GridIconRenderer::AddIcon(
    const glm::ivec2& position,
    const xc::Rotation90 rotation,
    const glm::vec3& color)
{
    m_VBO.AddQuad();
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributePosition, position);
    xg::VBOAddTextureUV(m_VBO, s_AttributeTextureUV, rotation);
    xg::VBOAdd(m_VBO, s_AttributeColor, color);
}

void xg::GridIconRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::GridIconRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& /*infoTextureSize*/,
    const GLuint /*infoTexture*/)
{
    if (m_VBO.IsEmpty())
    {
        return;
    }

    m_VBO.UpdateAndBindBuffers();

    glUseProgram(m_Program.GetProgramId());
    xg::GLEnableAlphaBlend();
    m_VBO.Bind();

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform1f(m_FeatherUniform, feather);
    glUniform1f(m_IconSizeUniform, m_IconSize);
    glUniform3fv(m_ColorUniform, 1, glm::value_ptr(m_Color));

    m_VBO.Draw();
}
