#include "WireLineRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"

namespace
{
    constexpr GLuint s_AttributeP1 = 0;
    constexpr GLuint s_AttributeP2 = 1;
    constexpr GLuint s_AttributeUV = 2;
    constexpr GLuint s_AttributeWireUV1 = 3;
    constexpr GLuint s_AttributeWireUV2 = 4;

    void SetUniform(GLint uniform, bool value)
    {
        glUniform1f(uniform, value ? 1.f : 0.f);
    }

    void SetUniform(GLint uniform, float value)
    {
        glUniform1f(uniform, value);
    }

    void SetUniform(GLint uniform, const glm::vec2& value)
    {
        glUniform2fv(uniform, 1, glm::value_ptr(value));
    }

    void SetUniform(GLint uniform, const glm::vec3& value)
    {
        glUniform3fv(uniform, 1, glm::value_ptr(value));
    }

    void SetUniform(GLint uniform, const glm::mat4& value)
    {
        glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(value));
    }
}

xg::WireLineRenderer::WireLineRenderer(const xc::ShaderProgram& program)
    : m_Program(program)
{
    m_ViewProjectionUniform = program.GetUniformLocation("u_ViewProjection");
    m_WireTextureSizeUniform = m_Program.GetUniformLocation("u_WireTextureSize");
    m_FeatherUniform = program.GetUniformLocation("u_Feather");

#define ADD_UNIFORM(TYPE, NAME, DEFAULT_VALUE) m_##NAME##Uniform = program.GetUniformLocation("u_" #NAME);
#include "WireLineRendererUniformList.h"
#undef ADD_UNIFORM

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
    const int distance = std::max(std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));

    m_VBO.AddQuad();
    xg::VBOAdd(m_VBO, s_AttributeP1, p1);
    xg::VBOAdd(m_VBO, s_AttributeP2, p2);
    xg::VBOAddQuadUV(m_VBO, s_AttributeUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV1, infoUV);
    xg::VBOAdd(m_VBO, s_AttributeWireUV2, infoUV + glm::ivec2(distance, 0));
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
    SetUniform(m_WireTextureSizeUniform, fWireTextureSize);
    SetUniform(m_ViewProjectionUniform, viewProjection);
    SetUniform(m_FeatherUniform, feather);

#define ADD_UNIFORM(TYPE, NAME, DEFAULT_VALUE) SetUniform(m_##NAME##Uniform, m_##NAME);
#include "WireLineRendererUniformList.h"
#undef ADD_UNIFORM

    // todo need to set this every frame?
    glBindTexture(GL_TEXTURE_2D, infoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    m_VBO.Draw();
}
