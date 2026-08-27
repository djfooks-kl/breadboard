#include "SelectionBoxRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"
#include "Rendering/UniformHelpers.h"

namespace
{
    constexpr GLuint s_AttributeTextureUV = 0;

    // -- [CODEGEN START] UniformsDefinitions(xg::SelectionBoxUniforms)
    void FillUniformLocations(const xc::ShaderProgram& program, xg::SelectionBoxUniformsLocations& out_uniformsLocations)
    {
        out_uniformsLocations.m_EdgeColor = program.GetUniformLocation("u_EdgeColor");
        out_uniformsLocations.m_FillColor = program.GetUniformLocation("u_FillColor");
        out_uniformsLocations.m_FillAlpha = program.GetUniformLocation("u_FillAlpha");
        out_uniformsLocations.m_EdgeWidth = program.GetUniformLocation("u_EdgeWidth");
        out_uniformsLocations.m_ViewProjection = program.GetUniformLocation("u_ViewProjection");
        out_uniformsLocations.m_Feather = program.GetUniformLocation("u_Feather");
        out_uniformsLocations.m_Min = program.GetUniformLocation("u_Min");
        out_uniformsLocations.m_Max = program.GetUniformLocation("u_Max");
    }

    void SetUniformValues(const xg::SelectionBoxUniformsLocations& uniformsLocations, const xg::SelectionBoxUniforms& uniforms)
    {
        xg::SetUniform(uniformsLocations.m_EdgeColor, uniforms.m_EdgeColor);
        xg::SetUniform(uniformsLocations.m_FillColor, uniforms.m_FillColor);
        xg::SetUniform(uniformsLocations.m_FillAlpha, uniforms.m_FillAlpha);
        xg::SetUniform(uniformsLocations.m_EdgeWidth, uniforms.m_EdgeWidth);
    }
    // -- [CODEGEN END]
}

xg::SelectionBoxRenderer::SelectionBoxRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    FillUniformLocations(program, m_UniformsLocations);

    m_VBO.AddIVertexAttribute(s_AttributeTextureUV, 2);
}

bool xg::SelectionBoxRenderer::HasBox() const
{
    return !m_VBO.IsEmpty();
}

void xg::SelectionBoxRenderer::AddBox()
{
    m_VBO.AddQuad();
    xg::VBOAddQuadUV(m_VBO, s_AttributeTextureUV);
}

void xg::SelectionBoxRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::SelectionBoxRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::vec2& p1,
    const glm::vec2& p2)
{
    if (m_VBO.IsEmpty())
    {
        return;
    }

    m_VBO.UpdateAndBindBuffers();

    glUseProgram(m_Program.GetProgramId());
    xg::GLEnableAlphaBlend();
    m_VBO.Bind();

    const glm::vec2 min = glm::min(p1, p2);
    const glm::vec2 max = glm::max(p1, p2);

    SetUniform(m_UniformsLocations.m_Min, min);
    SetUniform(m_UniformsLocations.m_Max, max);
    SetUniform(m_UniformsLocations.m_ViewProjection, viewProjection);
    SetUniform(m_UniformsLocations.m_Feather, feather);
    SetUniformValues(m_UniformsLocations, m_Uniforms);

    m_VBO.Draw();
}
