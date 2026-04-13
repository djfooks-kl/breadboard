#include "GridRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"

namespace
{
    constexpr GLuint s_AttributeUV = 0;
}

xg::GridRenderer::GridRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = glGetUniformLocation(m_Program.GetProgramId(), "viewProjection");
    m_BoxUniform = glGetUniformLocation(m_Program.GetProgramId(), "box");
    m_FeatherUniform = glGetUniformLocation(m_Program.GetProgramId(), "feather");
    m_SizeUniform = glGetUniformLocation(m_Program.GetProgramId(), "size");

    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);

    m_VBO.AddQuad();
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);

    m_VBO.UpdateAndBindBuffers();
}

void xg::GridRenderer::Draw(
    const glm::mat4& viewProjection,
    const glm::mat4& invViewProjection,
    const glm::ivec2& size,
    const float feather)
{
    glm::vec4 viewTopLeft(-1.f, -1.f, 0.f, 1.f);
    glm::vec4 projectedTopLeft = invViewProjection * viewTopLeft;
    glm::vec2 worldTopLeft = projectedTopLeft / projectedTopLeft.w;

    glm::vec4 viewBottomRight(1.f, 1.f, 0.f, 1.f);
    glm::vec4 projectedBottomRight = invViewProjection * viewBottomRight;
    glm::vec2 worldBottomRight = projectedBottomRight / projectedBottomRight.w;

    glm::vec4 minMaxWorld(worldTopLeft.x, worldTopLeft.y, worldBottomRight.x, worldBottomRight.y);

    glUseProgram(m_Program.GetProgramId());
    m_VBO.Bind();

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform4fv(m_BoxUniform, 1, glm::value_ptr(minMaxWorld));
    glUniform1f(m_FeatherUniform, feather);
    glm::vec2 fsize = size;
    glUniform2fv(m_SizeUniform, 1, glm::value_ptr(fsize));

    m_VBO.Draw();
}
