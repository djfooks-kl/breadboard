#include "SwitchRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RenderingFlags.h"

namespace
{
    constexpr int s_PositionNumComponents = 2;

    constexpr GLuint s_AttributeBaseP = 0;
    constexpr GLuint s_AttributeP1 = 1;
    constexpr GLuint s_AttributeP2 = 2;
    constexpr GLuint s_AttributeTextureUV = 3;
    constexpr GLuint s_AttributeWireUV = 4;

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

xg::SwitchRenderer::SwitchRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
    , m_HasInfoTexture(true)
    , m_InnerRadius(0.26f)
    , m_OuterRadius(0.31f)
    , m_InnerContactWidth(0.07f)
    , m_OuterContactWidth(0.14f)
{
    m_FeatherUniform = m_Program.GetUniformLocation("feather");
    m_WireTextureSizeUniform = m_Program.GetUniformLocation("wireTextureSize");
    m_ViewProjectionUniform = m_Program.GetUniformLocation("viewProjection");
    m_ColorUniform = m_Program.GetUniformLocation("outlineColor");
    m_InnerRadiusUniform = m_Program.GetUniformLocation("innerRadius");
    m_OuterRadiusUniform = m_Program.GetUniformLocation("outerRadius");
    m_InnerContactWidthUniform = m_Program.GetUniformLocation("innerContactWidth");
    m_OuterContactWidthUniform = m_Program.GetUniformLocation("outerContactWidth");
}

xg::SwitchRenderer::~SwitchRenderer()
{
    glDeleteVertexArrays(1, &m_VBO);
    glDeleteBuffers(1, &m_BasePBuffer);
    glDeleteBuffers(1, &m_P1Buffer);
    glDeleteBuffers(1, &m_P2Buffer);
    glDeleteBuffers(1, &m_TextureUVBuffer);
    glDeleteBuffers(1, &m_WireUVBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
}

void xg::SwitchRenderer::AddSwitch(
    const glm::ivec2& basePosition,
    const glm::ivec2& p1,
    const glm::ivec2& p2,
    const glm::ivec2& infoUV)
{
    const unsigned int i = static_cast<unsigned int>(m_BaseP.size()) / s_PositionNumComponents;

    m_BaseP.reserve(m_BaseP.size() + 8);
    m_BaseP.push_back(basePosition.x);
    m_BaseP.push_back(basePosition.y);
    m_BaseP.push_back(basePosition.x);
    m_BaseP.push_back(basePosition.y);
    m_BaseP.push_back(basePosition.x);
    m_BaseP.push_back(basePosition.y);
    m_BaseP.push_back(basePosition.x);
    m_BaseP.push_back(basePosition.y);

    m_P1.reserve(m_P1.size() + 8);
    m_P1.push_back(p1.x);
    m_P1.push_back(p1.y);
    m_P1.push_back(p1.x);
    m_P1.push_back(p1.y);
    m_P1.push_back(p1.x);
    m_P1.push_back(p1.y);
    m_P1.push_back(p1.x);
    m_P1.push_back(p1.y);

    m_P2.reserve(m_P2.size() + 8);
    m_P2.push_back(p2.x);
    m_P2.push_back(p2.y);
    m_P2.push_back(p2.x);
    m_P2.push_back(p2.y);
    m_P2.push_back(p2.x);
    m_P2.push_back(p2.y);
    m_P2.push_back(p2.x);
    m_P2.push_back(p2.y);

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

    m_WireUVs.reserve(m_WireUVs.size() + 8);
    m_WireUVs.push_back(infoUV.x);
    m_WireUVs.push_back(infoUV.y);
    m_WireUVs.push_back(infoUV.x);
    m_WireUVs.push_back(infoUV.y);
    m_WireUVs.push_back(infoUV.x);
    m_WireUVs.push_back(infoUV.y);
    m_WireUVs.push_back(infoUV.x);
    m_WireUVs.push_back(infoUV.y);

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

void xg::SwitchRenderer::RemoveAll()
{
    m_BaseP.clear();
    m_P1.clear();
    m_P2.clear();
    m_TextureUV.clear();
    m_WireUVs.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

void xg::SwitchRenderer::AddRenderable(
    const glm::ivec2& position,
    const xc::Rotation90 rotation,
    const int flags,
    const glm::ivec2& infoUV)
{
    if (flags == xg::rendering::s_SwitchSingle)
    {
        AddSwitch(
            position,
            position,
            position + rotation.GetIMatrix() * glm::ivec2(0, 1),
            infoUV);
    }
    else
    {
        AddSwitch(
            position + rotation.GetIMatrix() * glm::ivec2(0, 1),
            position,
            position + rotation.GetIMatrix() * glm::ivec2(0, 2),
            infoUV);
    }
}

void xg::SwitchRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& wireTextureSize,
    const GLuint texture)
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
        m_BasePBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_BaseP, m_BasePBuffer);
        m_P1Buffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_P1, m_P1Buffer);
        m_P2Buffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_P2, m_P2Buffer);
        m_TextureUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_TextureUV, m_TextureUVBuffer);
        m_WireUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_WireUVs, m_WireUVBuffer);
        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, m_BasePBuffer);
        glVertexAttribPointer(s_AttributeBaseP, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeBaseP);

        glBindBuffer(GL_ARRAY_BUFFER, m_P1Buffer);
        glVertexAttribPointer(s_AttributeP1, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeP1);

        glBindBuffer(GL_ARRAY_BUFFER, m_P2Buffer);
        glVertexAttribPointer(s_AttributeP2, 2, GL_INT, GL_FALSE, 2 * sizeof(int), (void*)0);
        glEnableVertexAttribArray(s_AttributeP2);

        glBindBuffer(GL_ARRAY_BUFFER, m_TextureUVBuffer);
        glVertexAttribPointer(s_AttributeTextureUV, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributeTextureUV);

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
    glUniform1f(m_InnerRadiusUniform, m_InnerRadius);
    glUniform1f(m_OuterRadiusUniform, m_OuterRadius);
    glUniform1f(m_InnerContactWidthUniform, m_InnerContactWidth);
    glUniform1f(m_OuterContactWidthUniform, m_OuterContactWidth);
    glUniform3fv(m_ColorUniform, 1, glm::value_ptr(m_Color));
    const glm::vec2 fWireTextureSize = m_HasInfoTexture ? wireTextureSize : glm::ivec2(0, 0);
    glUniform2fv(m_WireTextureSizeUniform, 1, glm::value_ptr(fWireTextureSize));

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
