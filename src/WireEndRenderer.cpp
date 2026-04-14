#include "WireEndRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"

namespace
{
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeUV = 1;
    constexpr GLuint s_AttributeWireUV = 2;
}

xg::WireEndRenderer::WireEndRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = program.GetUniformLocation("viewProjection");
    m_WireTextureSizeUniform = m_Program.GetUniformLocation("wireTextureSize");
    m_FeatherUniform = program.GetUniformLocation("feather");
    m_SizeUniform = program.GetUniformLocation("size");
    m_ColorEmptyUniform = program.GetUniformLocation("colorEmpty");
    m_ColorFullUniform = program.GetUniformLocation("colorFull");
    m_ExpandUniform = program.GetUniformLocation("expand");

    m_VBO.AddIVertexAttribute(s_AttributePosition, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV, 2);
}

void xg::WireEndRenderer::AddWireEnd(
    const glm::ivec2& position,
    const glm::ivec2& infoUV)
{
    m_VBO.AddQuad();
    xg::VBOAdd(m_VBO, s_AttributePosition, position);
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV, infoUV);
}

void xg::WireEndRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::WireEndRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& infoTextureSize,
    const GLuint infoTexture)
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
    glUniform1f(m_SizeUniform, m_Size);
    glUniform3fv(m_ColorEmptyUniform, 1, glm::value_ptr(m_ColorEmpty));
    glUniform3fv(m_ColorFullUniform, 1, glm::value_ptr(m_ColorFull));
    glUniform1f(m_ExpandUniform, m_Expand ? 1.f : 0.f);
    const glm::vec2 fWireTextureSize = infoTextureSize;
    glUniform2fv(m_WireTextureSizeUniform, 1, glm::value_ptr(fWireTextureSize));

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, infoTexture);

    m_VBO.Draw();
}
