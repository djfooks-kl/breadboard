#include "CogBoxRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"

namespace
{
    int s_PositionNumComponents = 2;

    constexpr GLuint s_AttributeTextureUV = 0;
    constexpr GLuint s_AttributeP1 = 1;
    constexpr GLuint s_AttributeP2 = 2;

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

xg::CogBoxRenderer::CogBoxRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_BorderUniform = glGetUniformLocation(m_Program.GetProgramId(), "border");
    m_ColorUniform = glGetUniformLocation(m_Program.GetProgramId(), "color");
    m_ExpandUniform = glGetUniformLocation(m_Program.GetProgramId(), "expand");
    m_FeatherUniform = glGetUniformLocation(m_Program.GetProgramId(), "feather");
    m_FillColorUniform = glGetUniformLocation(m_Program.GetProgramId(), "fillColor");
    m_ViewProjectionUniform = glGetUniformLocation(m_Program.GetProgramId(), "viewProjection");
}

xg::CogBoxRenderer::~CogBoxRenderer()
{
    glDeleteVertexArrays(1, &m_VBO);
    glDeleteBuffers(1, &m_P1Buffer);
    glDeleteBuffers(1, &m_P2Buffer);
    glDeleteBuffers(1, &m_TextureUVBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
}

void xg::CogBoxRenderer::AddBox(
    const glm::ivec2& p1,
    const glm::ivec2& p2)
{
    const unsigned int i = static_cast<unsigned int>(m_P1.size()) / s_PositionNumComponents;

    const glm::ivec2 min = glm::min(p1, p2);
    const glm::ivec2 max = glm::max(p1, p2);

    m_P1.reserve(m_P1.size() + 8);
    m_P1.push_back(min.x);
    m_P1.push_back(min.y);
    m_P1.push_back(min.x);
    m_P1.push_back(min.y);
    m_P1.push_back(min.x);
    m_P1.push_back(min.y);
    m_P1.push_back(min.x);
    m_P1.push_back(min.y);

    m_P2.reserve(m_P2.size() + 8);
    m_P2.push_back(max.x);
    m_P2.push_back(max.y);
    m_P2.push_back(max.x);
    m_P2.push_back(max.y);
    m_P2.push_back(max.x);
    m_P2.push_back(max.y);
    m_P2.push_back(max.x);
    m_P2.push_back(max.y);

    const float s0 = 0.f;
    const float t0 = 0.f;
    const float s1 = 1.f;
    const float t1 = 1.f;

    m_TextureUV.reserve(m_TextureUV.size() + 8);
    m_TextureUV.push_back(s0);
    m_TextureUV.push_back(t0);
    m_TextureUV.push_back(s1);
    m_TextureUV.push_back(t0);
    m_TextureUV.push_back(s0);
    m_TextureUV.push_back(t1);
    m_TextureUV.push_back(s1);
    m_TextureUV.push_back(t1);

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

void xg::CogBoxRenderer::RemoveAllBoxes()
{
    m_P1.clear();
    m_P2.clear();
    m_TextureUV.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

void xg::CogBoxRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather)
{
    if (m_P1.empty())
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
        m_P1Buffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_P1, m_P1Buffer);
        m_P2Buffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_P2, m_P2Buffer);
        m_TextureUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_TextureUV, m_TextureUVBuffer);
        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, m_P1Buffer);
        glVertexAttribPointer(s_AttributeP1, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeP1);

        glBindBuffer(GL_ARRAY_BUFFER, m_P2Buffer);
        glVertexAttribPointer(s_AttributeP2, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeP2);

        glBindBuffer(GL_ARRAY_BUFFER, m_TextureUVBuffer);
        glVertexAttribPointer(s_AttributeTextureUV, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributeTextureUV);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

        m_BuffersDirty = false;
    }

    glUseProgram(m_Program.GetProgramId());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindVertexArray(m_VBO);

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform1f(m_FeatherUniform, feather);
    glUniform1f(m_BorderUniform, m_Border);
    glUniform1f(m_ExpandUniform, m_Expand);
    glUniform3fv(m_ColorUniform, 1, glm::value_ptr(m_Color));
    glUniform3fv(m_FillColorUniform, 1, glm::value_ptr(m_FillColor));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
