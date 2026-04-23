#include "SwitchRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RenderingFlags.h"
#include "Rendering/RendererHelpers.h"

namespace
{
    constexpr GLuint s_AttributeBaseP = 0;
    constexpr GLuint s_AttributeP1 = 1;
    constexpr GLuint s_AttributeP2 = 2;
    constexpr GLuint s_AttributeUV = 3;
    constexpr GLuint s_AttributeWireUV = 4;
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

    m_VBO.AddIVertexAttribute(s_AttributeBaseP, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP2, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV, 2);
}

void xg::SwitchRenderer::AddSwitch(
    const glm::ivec2& basePosition,
    const glm::ivec2& p1,
    const glm::ivec2& p2,
    const glm::ivec2& infoUV)
{
    m_VBO.AddQuad();
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributeBaseP, basePosition);
    xg::VBOAdd(m_VBO, s_AttributeP1, p1);
    xg::VBOAdd(m_VBO, s_AttributeP2, p2);
    xg::VBOAdd(m_VBO, s_AttributeWireUV, infoUV);
}

void xg::SwitchRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::SwitchRenderer::AddRenderable(
    const xc::ITransform& transform,
    const int flags,
    const glm::ivec2& infoUV)
{
    if (flags == xg::rendering::s_SwitchSingle)
    {
        AddSwitch(
            transform.m_Translation,
            transform.m_Translation,
            transform.Apply(glm::ivec2(0, 1)),
            infoUV);
    }
    else
    {
        AddSwitch(
            transform.Apply(glm::ivec2(0, 1)),
            transform.m_Translation,
            transform.Apply(glm::ivec2(0, 2)),
            infoUV);
    }
}

void xg::SwitchRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& wireTextureSize,
    const GLuint texture)
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
    glUniform1f(m_InnerRadiusUniform, m_InnerRadius);
    glUniform1f(m_OuterRadiusUniform, m_OuterRadius);
    glUniform1f(m_InnerContactWidthUniform, m_InnerContactWidth);
    glUniform1f(m_OuterContactWidthUniform, m_OuterContactWidth);
    glUniform3fv(m_ColorUniform, 1, glm::value_ptr(m_Color));
    const glm::vec2 fWireTextureSize = m_HasInfoTexture ? wireTextureSize : glm::ivec2(0, 0);
    glUniform2fv(m_WireTextureSizeUniform, 1, glm::value_ptr(fWireTextureSize));

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, texture);

    m_VBO.Draw();
}
