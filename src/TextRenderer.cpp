#include "TextRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Font.h"
#include "Core/GLFWLib.h"
#include "Core/ShaderProgram.h"
#include "Rendering/RendererHelpers.h"

namespace
{
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureUV = 1;
    constexpr GLuint s_AttributeColor = 2;
}

xg::TextRenderer::TextRenderer(const xc::Font& font, const xc::ShaderProgram& program)
    : m_Font(font)
    , m_Program(program)
{
    m_VBO.AddFVertexAttribute(s_AttributePosition, 2);
    m_VBO.AddFVertexAttribute(s_AttributeTextureUV, 2);
    m_VBO.AddFVertexAttribute(s_AttributeColor, 3);
}

void xg::TextRenderer::AddString(const std::string& text, float size, float x, float y, const glm::vec3& color)
{
    for (const char c : text)
    {
        const float xAdvance = AddCharacter(c, size, x, y, color);
        x += xAdvance;
    }
}

void xg::TextRenderer::RemoveAllStrings()
{
    m_VBO.RemoveAllData();
}

float xg::TextRenderer::AddCharacter(
    char c,
    float size,
    float x,
    float y,
    const glm::vec3& color)
{
    // https://www.redblobgames.com/x/2403-distance-field-fonts/
    const auto shapeItr = m_Font.m_GlyphLayout.find(c);
    if (shapeItr == m_Font.m_GlyphLayout.end())
    {
        return 0.f;
    }
    const auto& shape = shapeItr->second;

    const float s0 = shape.m_X / m_Font.m_ScaleW;
    const float s1 = (shape.m_X + shape.m_Width) / m_Font.m_ScaleW;
    const float t1 = shape.m_Y / m_Font.m_ScaleH;
    const float t0 = (shape.m_Y + shape.m_Height) / m_Font.m_ScaleH;

    /* Placement of a character within the box:
     *
     *     +---------+  -- top of box @ lineHeight
     *     |         |
     *     +---------+  -- top of glyph @ lineHeight - yoffset
     *     |         |
     *     |         |
     *     |         |
     *     |         |
     *     |         |
     *     +---------+  -- bottom of glyph @ lineHeight - yoffset - height
     *     |         |
     *     +---------+  -- bottom of box @ 0
     *
     *
     */
    const float scale = size / m_Font.m_Size;
    const float x0 = x + scale * shape.m_XOffset; // left
    const float x1 = x + scale * (shape.m_XOffset + shape.m_Width); // right
    const float y0 = y + scale * (m_Font.m_LineHeight - shape.m_YOffset - shape.m_Height); // bottom
    const float y1 = y + scale * (m_Font.m_LineHeight - shape.m_YOffset); // top

    m_VBO.AddQuad();

    auto& positionVertexData = std::get<std::vector<float>>(m_VBO.ModifyVertexAttributeData(s_AttributePosition));
    positionVertexData.reserve(positionVertexData.size() + 8);
    positionVertexData.push_back(x0);
    positionVertexData.push_back(y0);
    positionVertexData.push_back(x1);
    positionVertexData.push_back(y0);
    positionVertexData.push_back(x0);
    positionVertexData.push_back(y1);
    positionVertexData.push_back(x1);
    positionVertexData.push_back(y1);

    auto& textureUVVertexData = std::get<std::vector<float>>(m_VBO.ModifyVertexAttributeData(s_AttributeTextureUV));
    textureUVVertexData.reserve(textureUVVertexData.size() + 8);
    textureUVVertexData.push_back(s0);
    textureUVVertexData.push_back(t0);
    textureUVVertexData.push_back(s1);
    textureUVVertexData.push_back(t0);
    textureUVVertexData.push_back(s0);
    textureUVVertexData.push_back(t1);
    textureUVVertexData.push_back(s1);
    textureUVVertexData.push_back(t1);

    xg::VBOAdd(m_VBO, s_AttributeColor, color);

    return shape.m_XAdvance * scale;
}

void xg::TextRenderer::Draw(const glm::mat4& viewProjection)
{
    if (m_VBO.IsEmpty())
    {
        return;
    }

    m_VBO.UpdateAndBindBuffers();

    glUseProgram(m_Program.GetProgramId());
    xg::GLEnableAlphaBlend();
    glBindTexture(GL_TEXTURE_2D, m_Font.m_GLTexture);
    m_VBO.Bind();

    GLint viewProjectionUniform = glGetUniformLocation(m_Program.GetProgramId(), "viewProjection");
    glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));

    m_VBO.Draw();
}
