#include "GridIconRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"

namespace
{
    int s_PositionNumComponents = 2;

    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeOffset = 1;
    constexpr GLuint s_AttributeTextureUV = 2;
    constexpr GLuint s_AttributeColor = 3 ;

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

xg::GridIconRenderer::GridIconRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = program.GetUniformLocation("viewProjection");
    m_FeatherUniform = program.GetUniformLocation("feather");
    m_IconSizeUniform = program.GetUniformLocation("iconSize");
}

xg::GridIconRenderer::~GridIconRenderer()
{
    glDeleteVertexArrays(1, &m_VBO);
    glDeleteBuffers(1, &m_PositionsBuffer);
    glDeleteBuffers(1, &m_OffsetsBuffer);
    glDeleteBuffers(1, &m_TextureUVBuffer);
    glDeleteBuffers(1, &m_ColorBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
}

void xg::GridIconRenderer::AddRenderable(
    const glm::ivec2& position,
    const xc::Rotation90 rotation)
{
    AddIcon(position, rotation, glm::vec3(0.f, 0.f, 0.f));
}

void xg::GridIconRenderer::AddIcon(
    const glm::ivec2& position,
    const xc::Rotation90 rotation,
    const glm::vec3& color)
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

    const int offsetS0 = 0;
    const int offsetT0 = 0;
    const int offsetS1 = 1;
    const int offsetT1 = 1;

    m_Offsets.reserve(m_Offsets.size() + 8);
    m_Offsets.push_back(offsetS0);
    m_Offsets.push_back(offsetT0);
    m_Offsets.push_back(offsetS1);
    m_Offsets.push_back(offsetT0);
    m_Offsets.push_back(offsetS0);
    m_Offsets.push_back(offsetT1);
    m_Offsets.push_back(offsetS1);
    m_Offsets.push_back(offsetT1);

    const xc::Rotation90UVs& uvs = rotation.GetUVs();

    m_TextureUV.reserve(m_TextureUV.size() + 8);
    m_TextureUV.push_back(uvs[0]);
    m_TextureUV.push_back(uvs[1]);
    m_TextureUV.push_back(uvs[2]);
    m_TextureUV.push_back(uvs[3]);
    m_TextureUV.push_back(uvs[4]);
    m_TextureUV.push_back(uvs[5]);
    m_TextureUV.push_back(uvs[6]);
    m_TextureUV.push_back(uvs[7]);

    m_Colors.reserve(m_Colors.size() + 12);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);

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

void xg::GridIconRenderer::RemoveAll()
{
    m_Positions.clear();
    m_TextureUV.clear();
    m_Colors.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

void xg::GridIconRenderer::Draw(const glm::mat4& viewProjection, const float feather)
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
        m_OffsetsBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Offsets, m_OffsetsBuffer);
        m_TextureUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_TextureUV, m_TextureUVBuffer);
        m_ColorBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Colors, m_ColorBuffer);
        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
        glVertexAttribPointer(s_AttributePosition, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributePosition);

        glBindBuffer(GL_ARRAY_BUFFER, m_OffsetsBuffer);
        glVertexAttribPointer(s_AttributeOffset, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeOffset);

        glBindBuffer(GL_ARRAY_BUFFER, m_TextureUVBuffer);
        glVertexAttribPointer(s_AttributeTextureUV, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeTextureUV);

        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
        glVertexAttribPointer(s_AttributeColor, 3, GL_INT, GL_FALSE, 3 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeColor);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

        m_BuffersDirty = false;
    }

    glUseProgram(m_Program.GetProgramId());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindVertexArray(m_VBO);

    glUniformMatrix4fv(m_ViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform1f(m_FeatherUniform, feather);
    glUniform1f(m_IconSizeUniform, m_IconSize);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
