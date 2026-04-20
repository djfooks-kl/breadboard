#include "WireLineRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"

namespace
{
    constexpr GLuint s_AttributeP1 = 0;
    constexpr GLuint s_AttributeP2 = 1;
    constexpr GLuint s_AttributeUV = 2;
    constexpr GLuint s_AttributeWireUV1 = 3;
    constexpr GLuint s_AttributeWireUV2 = 4;
}

xg::WireLineRenderer::WireLineRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = program.GetUniformLocation("viewProjection");
    m_WireTextureSizeUniform = m_Program.GetUniformLocation("wireTextureSize");
    m_FeatherUniform = program.GetUniformLocation("feather");
    m_InnerWidthUniform = program.GetUniformLocation("innerWidth");
    m_OuterWidthUniform = program.GetUniformLocation("outerWidth");
    m_ColorEmptyUniform = program.GetUniformLocation("colorEmpty");
    m_ColorFullUniform = program.GetUniformLocation("colorFull");
    m_ColorEdgeUniform = program.GetUniformLocation("colorEdge");
    m_ExpandUniform = program.GetUniformLocation("expand");

    m_VBO.AddIVertexAttribute(s_AttributeP1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP2, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV2, 2);
}

void xg::WireLineRenderer::AddWire(
    const glm::ivec2& p1,
    const glm::ivec2& p2,
    const glm::ivec2& infoUV)
{
    const int distance = std::max(std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));

    m_VBO.AddQuad();
    xg::VBOAdd(m_VBO, s_AttributeP1, p1);
    xg::VBOAdd(m_VBO, s_AttributeP2, p2);
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV1, infoUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV2, infoUV + glm::ivec2(distance, 0));
}

void xg::WireLineRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::WireLineRenderer::Draw(
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
    glUniform1f(m_InnerWidthUniform, m_InnerWidth);
    glUniform1f(m_OuterWidthUniform, m_OuterWidth);
    glUniform3fv(m_ColorEmptyUniform, 1, glm::value_ptr(m_ColorEmpty));
    glUniform3fv(m_ColorFullUniform, 1, glm::value_ptr(m_ColorFull));
    glUniform3fv(m_ColorEdgeUniform, 1, glm::value_ptr(m_ColorEdge));
    glUniform1f(m_ExpandUniform, m_Expand ? 1.f : 0.f);
    const glm::vec2 fWireTextureSize = infoTextureSize;
    glUniform2fv(m_WireTextureSizeUniform, 1, glm::value_ptr(fWireTextureSize));

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, infoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    m_VBO.Draw();
}
