#include "CogBoxVFXRenderer.h"

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
    constexpr GLuint s_AttributeVFXP1 = 3;
    constexpr GLuint s_AttributeVFXP2 = 4;
    constexpr GLuint s_AttributeVFXF1 = 5;
    constexpr GLuint s_AttributeVFXF2 = 6;

    // -- [CODEGEN START] UniformsDefinitions(xg::CogBoxVFXUniforms)
    void FillUniformLocations(const xc::ShaderProgram& program, xg::CogBoxVFXUniformsLocations& out_uniformsLocations)
    {
        out_uniformsLocations.m_Color = program.GetUniformLocation("u_Color");
        out_uniformsLocations.m_FillColor = program.GetUniformLocation("u_FillColor");
        out_uniformsLocations.m_Size = program.GetUniformLocation("u_Size");
        out_uniformsLocations.m_Expand = program.GetUniformLocation("u_Expand");
        out_uniformsLocations.m_ViewProjection = program.GetUniformLocation("u_ViewProjection");
        out_uniformsLocations.m_Feather = program.GetUniformLocation("u_Feather");
    }

    void SetUniformValues(const xg::CogBoxVFXUniformsLocations& uniformsLocations, const xg::CogBoxVFXUniforms& uniforms)
    {
        xg::SetUniform(uniformsLocations.m_Color, uniforms.m_Color);
        xg::SetUniform(uniformsLocations.m_FillColor, uniforms.m_FillColor);
        xg::SetUniform(uniformsLocations.m_Size, uniforms.m_Size);
        xg::SetUniform(uniformsLocations.m_Expand, uniforms.m_Expand);
    }
    // -- [CODEGEN END]
}

xg::CogBoxVFXRenderer::CogBoxVFXRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    FillUniformLocations(program, m_UniformsLocations);

    m_VBO.AddIVertexAttribute(s_AttributeTextureUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP2, 2);
    m_VBO.AddFVertexAttribute(s_AttributeVFXP1, 2);
    m_VBO.AddFVertexAttribute(s_AttributeVFXP2, 2);
    m_VBO.AddFVertexAttribute(s_AttributeVFXF1, 1);
    m_VBO.AddFVertexAttribute(s_AttributeVFXF2, 1);
}

void xg::CogBoxVFXRenderer::AddBox(
    const glm::ivec2& p1,
    const glm::ivec2& p2,
    const glm::vec2& vfxP1,
    const glm::vec2& vfxP2,
    const float vfxF1,
    const float vfxF2)
{
    const glm::ivec2 min = glm::min(p1, p2);
    const glm::ivec2 max = glm::max(p1, p2);

    m_VBO.AddQuad();
    xg::VBOAddQuadUV(m_VBO, s_AttributeTextureUV);
    xg::VBOAdd(m_VBO, s_AttributeP1, min);
    xg::VBOAdd(m_VBO, s_AttributeP2, max);
    xg::VBOAdd(m_VBO, s_AttributeVFXP1, vfxP1);
    xg::VBOAdd(m_VBO, s_AttributeVFXP2, vfxP2);
    xg::VBOAdd(m_VBO, s_AttributeVFXF1, vfxF1);
    xg::VBOAdd(m_VBO, s_AttributeVFXF2, vfxF2);
}

void xg::CogBoxVFXRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::CogBoxVFXRenderer::Draw(
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
