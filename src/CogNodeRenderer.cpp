#include "CogNodeRenderer.h"

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"
#include "Rendering/UniformHelpers.h"

namespace
{
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeUV = 1;
    constexpr GLuint s_AttributeWireUV = 2;

    // -- [CODEGEN START] UniformsDefinitions(xg::CogNodeUniforms)
    void FillUniformLocations(const xc::ShaderProgram& program, xg::CogNodeUniformsLocations& out_uniformsLocations)
    {
        out_uniformsLocations.m_RingColor = program.GetUniformLocation("u_RingColor");
        out_uniformsLocations.m_InnerRadius = program.GetUniformLocation("u_InnerRadius");
        out_uniformsLocations.m_OuterRadius = program.GetUniformLocation("u_OuterRadius");
        out_uniformsLocations.m_Radius = program.GetUniformLocation("u_Radius");
        out_uniformsLocations.m_ViewProjection = program.GetUniformLocation("u_ViewProjection");
        out_uniformsLocations.m_Feather = program.GetUniformLocation("u_Feather");
        out_uniformsLocations.m_WireTextureSize = program.GetUniformLocation("u_WireTextureSize");
    }

    void SetUniformValues(const xg::CogNodeUniformsLocations& uniformsLocations, const xg::CogNodeUniforms& uniforms)
    {
        xg::SetUniform(uniformsLocations.m_RingColor, uniforms.m_RingColor);
        xg::SetUniform(uniformsLocations.m_InnerRadius, uniforms.m_InnerRadius);
        xg::SetUniform(uniformsLocations.m_OuterRadius, uniforms.m_OuterRadius);
        xg::SetUniform(uniformsLocations.m_Radius, uniforms.m_Radius);
    }
    // -- [CODEGEN END]
}

xg::CogNodeRenderer::CogNodeRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    FillUniformLocations(program, m_UniformsLocations);

    m_VBO.AddIVertexAttribute(s_AttributePosition, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV, 2);
}

void xg::CogNodeRenderer::AddNode(
    const glm::ivec2& position,
    const glm::ivec2& wireUV)
{
    m_VBO.AddQuad();
    xg::VBOAdd(m_VBO, s_AttributePosition, position);
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV, wireUV);
}

void xg::CogNodeRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::CogNodeRenderer::AddRenderable(
    const xc::ITransform& transform,
    const int /*flags*/,
    const glm::ivec2& infoUV)
{
    AddNode(transform.m_Translation, infoUV);
}

void xg::CogNodeRenderer::Draw(
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
