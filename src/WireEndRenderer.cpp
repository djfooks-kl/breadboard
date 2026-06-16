#include "WireEndRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"
#include "Rendering/UniformHelpers.h"

namespace
{
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeUV = 1;
    constexpr GLuint s_AttributeWireUV = 2;

    // -- [CODEGEN START] UniformsDefinitions(xg::WireEndUniforms)
    void FillUniformLocations(const xc::ShaderProgram& program, xg::WireEndUniformsLocations& out_uniformsLocations)
    {
        out_uniformsLocations.m_ColorEmpty = program.GetUniformLocation("u_ColorEmpty");
        out_uniformsLocations.m_ColorFull = program.GetUniformLocation("u_ColorFull");
        out_uniformsLocations.m_Size = program.GetUniformLocation("u_Size");
        out_uniformsLocations.m_HasInfoTexture = program.GetUniformLocation("u_HasInfoTexture");
        out_uniformsLocations.m_Expand = program.GetUniformLocation("u_Expand");
        out_uniformsLocations.m_ViewProjection = program.GetUniformLocation("u_ViewProjection");
        out_uniformsLocations.m_Feather = program.GetUniformLocation("u_Feather");
        out_uniformsLocations.m_WireTextureSize = program.GetUniformLocation("u_WireTextureSize");
    }

    void SetUniformValues(const xg::WireEndUniformsLocations& uniformsLocations, const xg::WireEndUniforms& uniforms)
    {
        xg::SetUniform(uniformsLocations.m_ColorEmpty, uniforms.m_ColorEmpty);
        xg::SetUniform(uniformsLocations.m_ColorFull, uniforms.m_ColorFull);
        xg::SetUniform(uniformsLocations.m_Size, uniforms.m_Size);
        xg::SetUniform(uniformsLocations.m_HasInfoTexture, uniforms.m_HasInfoTexture);
        xg::SetUniform(uniformsLocations.m_Expand, uniforms.m_Expand);
    }
    // -- [CODEGEN END]
}

xg::WireEndRenderer::WireEndRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    FillUniformLocations(program, m_UniformsLocations);

    m_VBO.AddIVertexAttribute(s_AttributePosition, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV, 2);
}

void xg::WireEndRenderer::AddWireEnd(
    const glm::ivec2& position,
    const glm::ivec2& infoUV)
{
    m_VBO.AddQuad();
    xg::VBOAdd(m_VBO, s_AttributePosition, position);
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV, infoUV);
}

void xg::WireEndRenderer::SetValid(bool valid)
{
    m_Valid = valid;
}

void xg::WireEndRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::WireEndRenderer::Draw(
    const glm::mat4& viewProjection,
    const float feather,
    const glm::ivec2& infoTextureSize,
    const GLuint infoTexture)
{
    if (m_VBO.IsEmpty())
    {
        return;
    }

    m_VBO.UpdateAndBindBuffers();

    glUseProgram(m_Program.GetProgramId());
    xg::GLEnableAlphaBlend();
    m_VBO.Bind();

    m_Uniforms.m_ColorEmpty = m_Valid ? m_ColorValid : m_ColorInvalid;
    const glm::vec2 fWireTextureSize = infoTextureSize;
    SetUniform(m_UniformsLocations.m_WireTextureSize, fWireTextureSize);
    SetUniform(m_UniformsLocations.m_ViewProjection, viewProjection);
    SetUniform(m_UniformsLocations.m_Feather, feather);
    SetUniformValues(m_UniformsLocations, m_Uniforms);

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, infoTexture);

    m_VBO.Draw();
}
