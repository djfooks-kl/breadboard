#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

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
        ~GridRenderer();

        void Draw(
            const glm::mat4& viewProjection,
            const glm::mat4& invViewProjection,
            const glm::ivec2& size,
            const float feather);

    private:
        const xc::ShaderProgram& m_Program;

        std::vector<float> m_Positions;
        std::vector<unsigned int> m_Indices;

        GLuint m_PositionsBuffer = 0;
        GLuint m_IndicesBuffer = 0;
        GLuint m_VBO = 0;

        GLint m_ViewProjectionUniform = -1;
        GLint m_BoxUniform = -1;
        GLint m_FeatherUniform = -1;
        GLint m_SizeUniform = -1;
    };
}