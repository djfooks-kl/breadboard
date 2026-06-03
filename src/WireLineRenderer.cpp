#include "WireLineRenderer.h"

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "GridHelpers.h"
#include "Rendering/RendererHelpers.h"
#include "Rendering/UniformHelpers.h"

namespace
{
    constexpr GLuint s_AttributeP1 = 0;
    constexpr GLuint s_AttributeP2 = 1;
    constexpr GLuint s_AttributeUV = 2;
    constexpr GLuint s_AttributeWireUV1 = 3;
    constexpr GLuint s_AttributeWireUV2 = 4;

    // -- [CODEGEN START] UniformsDefinitions(xg::WireLineUniforms)
    void FillUniformLocations(const xc::ShaderProgram& program, xg::WireLineUniformsLocations& out_uniformsLocations)
    {
        out_uniformsLocations.m_ColorEmpty = program.GetUniformLocation("u_ColorEmpty");
        out_uniformsLocations.m_ColorFull = program.GetUniformLocation("u_ColorFull");
        out_uniformsLocations.m_ColorEdge = program.GetUniformLocation("u_ColorEdge");
        out_uniformsLocations.m_HasInfoTexture = program.GetUniformLocation("u_HasInfoTexture");
        out_uniformsLocations.m_Expand = program.GetUniformLocation("u_Expand");
        out_uniformsLocations.m_InnerWidth = program.GetUniformLocation("u_InnerWidth");
        out_uniformsLocations.m_OuterWidth = program.GetUniformLocation("u_OuterWidth");
        out_uniformsLocations.m_ViewProjection = program.GetUniformLocation("u_ViewProjection");
        out_uniformsLocations.m_Feather = program.GetUniformLocation("u_Feather");
        out_uniformsLocations.m_WireTextureSize = program.GetUniformLocation("u_WireTextureSize");
    }

    void SetUniformValues(const xg::WireLineUniformsLocations& uniformsLocations, const xg::WireLineUniforms& uniforms)
    {
        xg::SetUniform(uniformsLocations.m_ColorEmpty, uniforms.m_ColorEmpty);
        xg::SetUniform(uniformsLocations.m_ColorFull, uniforms.m_ColorFull);
        xg::SetUniform(uniformsLocations.m_ColorEdge, uniforms.m_ColorEdge);
        xg::SetUniform(uniformsLocations.m_HasInfoTexture, uniforms.m_HasInfoTexture);
        xg::SetUniform(uniformsLocations.m_Expand, uniforms.m_Expand);
        xg::SetUniform(uniformsLocations.m_InnerWidth, uniforms.m_InnerWidth);
        xg::SetUniform(uniformsLocations.m_OuterWidth, uniforms.m_OuterWidth);
    }
    // -- [CODEGEN END]
}

xg::WireLineRenderer::WireLineRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    FillUniformLocations(program, m_UniformsLocations);

    m_VBO.AddIVertexAttribute(s_AttributeP1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeP2, 2);
    m_VBO.AddIVertexAttribute(s_AttributeUV, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV1, 2);
    m_VBO.AddIVertexAttribute(s_AttributeWireUV2, 2);
}

void xg::WireLineRenderer::AddWire(
    const glm::ivec2& p1,
    const glm::ivec2& p2,
    const glm::ivec2& infoUV)
{
    const int cellDistance = xg::ChebyshevDistance(p1, p2);

    m_VBO.AddQuad();
    xg::VBOAdd(m_VBO, s_AttributeP1, p1);
    xg::VBOAdd(m_VBO, s_AttributeP2, p2);
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV1, infoUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV2, infoUV + glm::ivec2(cellDistance, 0));
}

void xg::WireLineRenderer::RemoveAll()
{
    m_VBO.RemoveAllData();
}

void xg::WireLineRenderer::Draw(
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

    const glm::vec2 fWireTextureSize = infoTextureSize;
    SetUniform(m_UniformsLocations.m_WireTextureSize, fWireTextureSize);
    SetUniform(m_UniformsLocations.m_ViewProjection, viewProjection);
    SetUniform(m_UniformsLocations.m_Feather, feather);
    SetUniformValues(m_UniformsLocations, m_Uniforms);

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, infoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    m_VBO.Draw();
}
