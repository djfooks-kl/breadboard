#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

#include "Rendering/VertexBufferObject.h"

typedef unsigned int GLuint;
typedef int GLint;

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct GridRenderer
    {
        GridRenderer(const xc::ShaderProgram& program);

        void Draw(
            const glm::mat4& viewProjection,
            const glm::mat4& invViewProjection,
            const glm::ivec2& size,
            const float feather);

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        GLint m_ViewProjectionUniform = -1;
        GLint m_BoxUniform = -1;
        GLint m_FeatherUniform = -1;
        GLint m_SizeUniform = -1;
    };
}