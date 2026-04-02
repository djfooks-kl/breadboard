#include "CogBoxRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/VBOHelpers.h"

namespace
{
    constexpr GLuint s_AttributeTextureUV = 0;
    constexpr GLuint s_AttributeP1 = 1;
    constexpr GLuint s_AttributeP2 = 2;
}

xg::CogBoxRenderer::CogBoxRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_BorderUniform = glGetUniformLocation(m_Program.GetProgramId(), "border");
    m_ColorUniform = glGetUniformLocation(m_Program.GetProgramId(), "color");
    m_ExpandUniform = glGetUniformLocation(m_Program.GetProgramId(), "expand");
    m_FeatherUniform = glGetUniformLocation(m_Program.GetProgramId(), "feather");
    m_FillColorUniform = glGetUniformLocation(m_Program.GetProgramId(), "fillColor");
    m_ViewProjectionUniform = glGetUniformLocation(m_Program.GetProgramId(), "viewProjection");

    m_VBO.AddIVertexAttribute(s_AttributeTextureUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP2, 2);
}

void xg::CogBoxRenderer::AddBox(
    const glm::ivec2& p1,
    const glm::ivec2& p2)
{
    const glm::ivec2 min = glm::min(p1, p2);
    const glm::ivec2 max = glm::max(p1, p2);

    m_VBO.AddQuad();
    xg::VBOAddQuadUV(m_VBO, s_AttributeTextureUV);
    xg::VBOAdd(m_VBO, s_AttributeP1, min);
    xg::VBOAdd(m_VBO, s_AttributeP2, max);
}

void xg::CogBoxRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::CogBoxRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather)
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
    glUniform1f(m_BorderUniform, m_Border);
    glUniform1f(m_ExpandUniform, m_Expand);
    glUniform3fv(m_ColorUniform, 1, glm::value_ptr(m_Color));
    glUniform3fv(m_FillColorUniform, 1, glm::value_ptr(m_FillColor));

    m_VBO.Draw();
}
