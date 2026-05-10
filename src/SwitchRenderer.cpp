#include "SwitchRenderer.h"

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RenderingFlags.h"
#include "Rendering/RendererHelpers.h"
#include "Rendering/UniformHelpers.h"

namespace
{
    constexpr GLuint s_AttributeBaseP = 0;
    constexpr GLuint s_AttributeP1 = 1;
    constexpr GLuint s_AttributeP2 = 2;
    constexpr GLuint s_AttributeUV = 3;
    constexpr GLuint s_AttributeWireUV = 4;

    // -- [CODEGEN START] UniformsDefinitions(xg::SwitchUniforms)
    void FillUniformLocations(const xc::ShaderProgram& program, xg::SwitchUniformsLocations& out_uniformsLocations)
    {
        out_uniformsLocations.m_OutlineColor = program.GetUniformLocation("u_OutlineColor");
        out_uniformsLocations.m_HasInfoTexture = program.GetUniformLocation("u_HasInfoTexture");
        out_uniformsLocations.m_InnerRadius = program.GetUniformLocation("u_InnerRadius");
        out_uniformsLocations.m_OuterRadius = program.GetUniformLocation("u_OuterRadius");
        out_uniformsLocations.m_InnerContactWidth = program.GetUniformLocation("u_InnerContactWidth");
        out_uniformsLocations.m_OuterContactWidth = program.GetUniformLocation("u_OuterContactWidth");
        out_uniformsLocations.m_ViewProjection = program.GetUniformLocation("u_ViewProjection");
        out_uniformsLocations.m_Feather = program.GetUniformLocation("u_Feather");
        out_uniformsLocations.m_WireTextureSize = program.GetUniformLocation("u_WireTextureSize");
    }

    void SetUniformValues(const xg::SwitchUniformsLocations& uniformsLocations, const xg::SwitchUniforms& uniforms)
    {
        xg::SetUniform(uniformsLocations.m_OutlineColor, uniforms.m_OutlineColor);
        xg::SetUniform(uniformsLocations.m_HasInfoTexture, uniforms.m_HasInfoTexture);
        xg::SetUniform(uniformsLocations.m_InnerRadius, uniforms.m_InnerRadius);
        xg::SetUniform(uniformsLocations.m_OuterRadius, uniforms.m_OuterRadius);
        xg::SetUniform(uniformsLocations.m_InnerContactWidth, uniforms.m_InnerContactWidth);
        xg::SetUniform(uniformsLocations.m_OuterContactWidth, uniforms.m_OuterContactWidth);
    }
    // -- [CODEGEN END]
}

xg::SwitchRenderer::SwitchRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    FillUniformLocations(program, m_UniformsLocations);

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

    const glm::vec2 fWireTextureSize = wireTextureSize;
    SetUniform(m_UniformsLocations.m_WireTextureSize, fWireTextureSize);
    SetUniform(m_UniformsLocations.m_ViewProjection, viewProjection);
    SetUniform(m_UniformsLocations.m_Feather, feather);
    SetUniformValues(m_UniformsLocations, m_Uniforms);

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, texture);

    m_VBO.Draw();
}
