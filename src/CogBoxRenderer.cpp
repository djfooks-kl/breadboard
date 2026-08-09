#include "CogBoxRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"
#include "Rendering/UniformHelpers.h"

namespace
{
    constexpr GLuint s_AttributeTextureUV = 0;
    constexpr GLuint s_AttributeP1 = 1;
    constexpr GLuint s_AttributeP2 = 2;

    // -- [CODEGEN START] UniformsDefinitions(xg::CogBoxUniforms)
    void FillUniformLocations(const xc::ShaderProgram& program, xg::CogBoxUniformsLocations& out_uniformsLocations)
    {
        out_uniformsLocations.m_Color = program.GetUniformLocation("u_Color");
        out_uniformsLocations.m_FillColor = program.GetUniformLocation("u_FillColor");
        out_uniformsLocations.m_Size = program.GetUniformLocation("u_Size");
        out_uniformsLocations.m_Expand = program.GetUniformLocation("u_Expand");
        out_uniformsLocations.m_ViewProjection = program.GetUniformLocation("u_ViewProjection");
        out_uniformsLocations.m_Feather = program.GetUniformLocation("u_Feather");
    }

    void SetUniformValues(const xg::CogBoxUniformsLocations& uniformsLocations, const xg::CogBoxUniforms& uniforms)
    {
        xg::SetUniform(uniformsLocations.m_Color, uniforms.m_Color);
        xg::SetUniform(uniformsLocations.m_FillColor, uniforms.m_FillColor);
        xg::SetUniform(uniformsLocations.m_Size, uniforms.m_Size);
        xg::SetUniform(uniformsLocations.m_Expand, uniforms.m_Expand);
    }
    // -- [CODEGEN END]
}

xg::CogBoxRenderer::CogBoxRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    FillUniformLocations(program, m_UniformsLocations);

    m_VBO.AddIVertexAttribute(s_AttributeTextureUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP2, 2);
}

void xg::CogBoxRenderer::AddBox(
    const glm::ivec2& p1,
    const glm::ivec2& p2)
{
    const glm::ivec2 min = glm::min(p1, p2);
    const glm::ivec2 max = glm::max(p1, p2);

    m_VBO.AddQuad();
    xg::VBOAddQuadUV(m_VBO, s_AttributeTextureUV);
    xg::VBOAdd(m_VBO, s_AttributeP1, min);
    xg::VBOAdd(m_VBO, s_AttributeP2, max);
}

void xg::CogBoxRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::CogBoxRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather)
{
    if (m_VBO.IsEmpty())
    {
        return;
    }

    m_VBO.UpdateAndBindBuffers();

    glUseProgram(m_Program.GetProgramId());
    xg::GLEnableAlphaBlend();
    m_VBO.Bind();

    SetUniform(m_UniformsLocations.m_ViewProjection, viewProjection);
    SetUniform(m_UniformsLocations.m_Feather, feather);
    SetUniformValues(m_UniformsLocations, m_Uniforms);

    m_VBO.Draw();
}
