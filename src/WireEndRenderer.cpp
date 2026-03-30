#include "WireEndRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"

namespace
{
    int s_PositionNumComponents = 2;

    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeUV = 1;
    constexpr GLuint s_AttributeWireUV = 2;

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

xg::WireEndRenderer::WireEndRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = program.GetUniformLocation("viewProjection");
    m_FeatherUniform = program.GetUniformLocation("feather");
    m_SizeUniform = program.GetUniformLocation("size");
    m_ColorUniform = program.GetUniformLocation("color");
}

xg::WireEndRenderer::~WireEndRenderer()
{
    glDeleteVertexArrays(1, &m_VBO);
    glDeleteBuffers(1, &m_PositionsBuffer);
    glDeleteBuffers(1, &m_UVBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
}

void xg::WireEndRenderer::AddWireEnd(
    const glm::ivec2& position,
    const glm::ivec2& infoUV)
{
    const unsigned int i = static_cast<unsigned int>(m_Positions.size()) / s_PositionNumComponents;

    m_Positions.reserve(m_Positions.size() + 8);
    m_Positions.push_back(position.x);
    m_Positions.push_back(position.y);
    m_Positions.push_back(position.x);
    m_Positions.push_back(position.y);
    m_Positions.push_back(position.x);
    m_Positions.push_back(position.y);
    m_Positions.push_back(position.x);
    m_Positions.push_back(position.y);

    const int s0 = 0;
    const int t0 = 0;
    const int s1 = 1;
    const int t1 = 1;

    m_UV.reserve(m_UV.size() + 8);
    m_UV.push_back(s0);
    m_UV.push_back(t0);
    m_UV.push_back(s1);
    m_UV.push_back(t0);
    m_UV.push_back(s0);
    m_UV.push_back(t1);
    m_UV.push_back(s1);
    m_UV.push_back(t1);

    //   2---3
    //   | \ |
    //   0---1
    // anti-clockwise winding
    m_Indices.reserve(m_Indices.size() + 6);
    m_Indices.push_back(i+0);
    m_Indices.push_back(i+1);
    m_Indices.push_back(i+2);
    m_Indices.push_back(i+1);
    m_Indices.push_back(i+2);
    m_Indices.push_back(i+3);

    m_BuffersDirty = true;
}

void xg::WireEndRenderer::RemoveAll()
{
    m_Positions.clear();
    m_UV.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

void xg::WireEndRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& infoTextureSize,
    const GLuint infoTexture)
{
    if (m_Positions.empty())
    {
        return;
    }

    if (m_BuffersDirty)
    {
        if (m_VBO == 0)
        {
            glGenVertexArrays(1, &m_VBO);
        }

        glBindVertexArray(m_VBO);
        m_PositionsBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Positions, m_PositionsBuffer);
        m_UVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_UV, m_UVBuffer);
        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
        glVertexAttribPointer(s_AttributePosition, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributePosition);

        glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer);
        glVertexAttribPointer(s_AttributeUV, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeUV);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

        m_BuffersDirty = false;
    }

    glUseProgram(m_Program.GetProgramId());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindVertexArray(m_VBO);

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform1f(m_FeatherUniform, feather);
    glUniform1f(m_SizeUniform, m_Size);
    glUniform3fv(m_ColorUniform, 1, glm::value_ptr(m_Color));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
