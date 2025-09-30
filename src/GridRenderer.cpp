#include "GridRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"

namespace
{
    constexpr GLuint s_AttributePosition = 0;

    template<typename TDATA>
    GLuint TryCreateAndBindBuffer(const GLenum target, const std::vector<TDATA>& data, GLuint buffer)
    {
        if (!buffer)
        {
            glGenBuffers(1, &buffer);
        }
        glBindBuffer(target, buffer);
        glBufferData(target, data.size() * sizeof(TDATA), &data.front(), GL_STATIC_DRAW);
        return buffer;
    }
}

GridRenderer::GridRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = glGetUniformLocation(m_Program.GetProgramId(), "viewProjection");
    m_BoxUniform = glGetUniformLocation(m_Program.GetProgramId(), "box");
    m_FeatherUniform = glGetUniformLocation(m_Program.GetProgramId(), "feather");

    const float x0 = 0.f;
    const float y0 = 0.f;
    const float x1 = 1.f;
    const float y1 = 1.f;

    m_Positions.reserve(8);
    m_Positions.push_back(x0);
    m_Positions.push_back(y0);
    m_Positions.push_back(x1);
    m_Positions.push_back(y0);
    m_Positions.push_back(x0);
    m_Positions.push_back(y1);
    m_Positions.push_back(x1);
    m_Positions.push_back(y1);

    //   2---3
    //   | \ |
    //   0---1
    // anti-clockwise winding
    m_Indices.reserve(6);
    m_Indices.push_back(0);
    m_Indices.push_back(1);
    m_Indices.push_back(2);
    m_Indices.push_back(1);
    m_Indices.push_back(2);
    m_Indices.push_back(3);

    glGenVertexArrays(1, &m_VBO);

    glBindVertexArray(m_VBO);
    m_PositionsBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Positions, m_PositionsBuffer);
    m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
    glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(s_AttributePosition);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);
}

GridRenderer::~GridRenderer()
{
    glDeleteVertexArrays(1, &m_VBO);
    glDeleteBuffers(1, &m_PositionsBuffer);
}

void GridRenderer::Draw(
    const glm::mat4& viewProjection,
    const glm::mat4& invViewProjection,
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
    glBindVertexArray(m_VBO);

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform4fv(m_BoxUniform, 1, glm::value_ptr(minMaxWorld));
    glUniform1f(m_FeatherUniform, feather);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
