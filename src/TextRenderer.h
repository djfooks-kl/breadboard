#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "Rendering/VertexBufferObject.h"

typedef unsigned int GLuint;

namespace xc
{
    struct Font;

    class ShaderProgram;
}

namespace xg
{
    struct TextRenderer
    {
        TextRenderer(const xc::Font& font, const xc::ShaderProgram& program);

        void Draw(const glm::mat4& viewProjection);

        void AddString(
            const std::string& text,
            float size,
            float x,
            float y,
            const glm::vec3& color);
        void RemoveAllStrings();

    private:
        float AddCharacter(
            char c,
            float size,
            float x,
            float y,
            const glm::vec3& color);

        const xc::Font& m_Font;
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;
    };
}