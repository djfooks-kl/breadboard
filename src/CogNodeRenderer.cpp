#include "CogNodeRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"

namespace
{
    int s_PositionNumComponents = 2;

    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeCircle = 1;
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

xg::CogNodeRenderer::CogNodeRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_FeatherUniform = m_Program.GetUniformLocation("feather");
    m_WireTextureSizeUniform = m_Program.GetUniformLocation("wireTextureSize");
    m_ViewProjectionUniform = m_Program.GetUniformLocation("viewProjection");
    m_RadiusUniform = m_Program.GetUniformLocation("radius");
    m_RingColorUniform = m_Program.GetUniformLocation("ringColor");

    m_Radius = 0.7f;
}

xg::CogNodeRenderer::~CogNodeRenderer()
{
    glDeleteVertexArrays(1, &m_VBO);
    glDeleteBuffers(1, &m_PositionBuffer);
    glDeleteBuffers(1, &m_CircleBuffer);
    glDeleteBuffers(1, &m_WireUVBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
}

void xg::CogNodeRenderer::AddNode(
    const glm::ivec2& position,
    const glm::ivec2& wireUV)
{
    const unsigned int i = static_cast<unsigned int>(m_Positions.size()) / s_PositionNumComponents;

    m_Circles.reserve(m_Circles.size() + 8);
    m_Circles.push_back(position.x);
    m_Circles.push_back(position.y);
    m_Circles.push_back(position.x);
    m_Circles.push_back(position.y);
    m_Circles.push_back(position.x);
    m_Circles.push_back(position.y);
    m_Circles.push_back(position.x);
    m_Circles.push_back(position.y);

    m_WireUVs.reserve(m_WireUVs.size() + 8);
    m_WireUVs.push_back(wireUV.x);
    m_WireUVs.push_back(wireUV.y);
    m_WireUVs.push_back(wireUV.x);
    m_WireUVs.push_back(wireUV.y);
    m_WireUVs.push_back(wireUV.x);
    m_WireUVs.push_back(wireUV.y);
    m_WireUVs.push_back(wireUV.x);
    m_WireUVs.push_back(wireUV.y);

    const int s0 = 0;
    const int t0 = 0;
    const int s1 = 1;
    const int t1 = 1;

    m_Positions.reserve(m_Positions.size() + 8);
    m_Positions.push_back(s0);
    m_Positions.push_back(t0);
    m_Positions.push_back(s1);
    m_Positions.push_back(t0);
    m_Positions.push_back(s0);
    m_Positions.push_back(t1);
    m_Positions.push_back(s1);
    m_Positions.push_back(t1);

    //   2---3
    //   | \ |
    //   0---1
    // anti-clockwise winding
    m_Indices.reserve(m_Indices.size() + 6);
    m_Indices.push_back(i + 0);
    m_Indices.push_back(i + 1);
    m_Indices.push_back(i + 2);
    m_Indices.push_back(i + 1);
    m_Indices.push_back(i + 2);
    m_Indices.push_back(i + 3);

    m_BuffersDirty = true;
}

void xg::CogNodeRenderer::RemoveAllNodes()
{
    m_Positions.clear();
    m_Circles.clear();
    m_WireUVs.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

void xg::CogNodeRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& wireTextureSize,
    const GLuint texture)
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
        m_PositionBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Positions, m_PositionBuffer);
        m_CircleBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Circles, m_CircleBuffer);
        m_WireUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_WireUVs, m_WireUVBuffer);
        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, m_PositionBuffer);
        glVertexAttribPointer(s_AttributePosition, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributePosition);

        glBindBuffer(GL_ARRAY_BUFFER, m_CircleBuffer);
        glVertexAttribPointer(s_AttributeCircle, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeCircle);

        glBindBuffer(GL_ARRAY_BUFFER, m_WireUVBuffer);
        glVertexAttribPointer(s_AttributeWireUV, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeWireUV);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

        m_BuffersDirty = false;
    }

    glUseProgram(m_Program.GetProgramId());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindVertexArray(m_VBO);

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform1f(m_FeatherUniform, feather);
    glUniform3fv(m_RingColorUniform, 1, glm::value_ptr(m_RingColor));
    const glm::vec2 fWireTextureSize = wireTextureSize;
    glUniform2fv(m_WireTextureSizeUniform, 1, glm::value_ptr(fWireTextureSize));
    glUniform1f(m_RadiusUniform, m_Radius);

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
