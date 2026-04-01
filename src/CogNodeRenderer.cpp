#include "CogNodeRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/VBOHelpers.h"

namespace
{
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeUV = 1;
    constexpr GLuint s_AttributeWireUV = 2;
}

xg::CogNodeRenderer::CogNodeRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_FeatherUniform = m_Program.GetUniformLocation("feather");
    m_WireTextureSizeUniform = m_Program.GetUniformLocation("wireTextureSize");
    m_ViewProjectionUniform = m_Program.GetUniformLocation("viewProjection");
    m_RadiusUniform = m_Program.GetUniformLocation("radius");
    m_RingColorUniform = m_Program.GetUniformLocation("ringColor");

    m_VBO.AddIVertexAttribute(s_AttributePosition, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV, 2);
}

void xg::CogNodeRenderer::AddNode(
    const glm::ivec2& position,
    const glm::ivec2& wireUV)
{
    m_VBO.AddQuad();
    xg::VBOAddIPosition2D(m_VBO, s_AttributePosition, position);
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAddIPosition2D(m_VBO, s_AttributeWireUV, wireUV);
}

void xg::CogNodeRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::CogNodeRenderer::AddRenderable(
    const glm::ivec2& position,
    const xc::Rotation90,
    const int /*flags*/,
    const glm::ivec2& infoUV)
{
    AddNode(position, infoUV);
}

void xg::CogNodeRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& wireTextureSize,
    const GLuint texture)
{
    if (m_VBO.IsEmpty())
    {
        return;
    }

    m_VBO.UpdateAndBindBuffers();

    glUseProgram(m_Program.GetProgramId());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    m_VBO.Bind();

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform1f(m_FeatherUniform, feather);
    glUniform3fv(m_RingColorUniform, 1, glm::value_ptr(m_RingColor));
    const glm::vec2 fWireTextureSize = wireTextureSize;
    glUniform2fv(m_WireTextureSizeUniform, 1, glm::value_ptr(fWireTextureSize));
    glUniform1f(m_RadiusUniform, m_Radius);

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, texture);

    m_VBO.Draw();
}
