#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/Rotation90.h"

typedef int GLint;
typedef unsigned int GLuint;

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct GridIconRenderer
    {
        GridIconRenderer(const xc::ShaderProgram& program);
        ~GridIconRenderer();

        void SetIconSize(const float v) { m_IconSize = v; }

        void Draw(const glm::mat4& viewProjection, const float feather);

        void AddIcon(
            const glm::ivec2& position,
            const xc::Rotation90 rotation,
            const glm::vec3& color);

        void RemoveAllIcons();

    private:
        const xc::ShaderProgram& m_Program;
        float m_IconSize = 1.f;

        GLint m_ViewProjectionUniform = -1;
        GLint m_FeatherUniform = -1;
        GLint m_IconSizeUniform = -1;

        std::vector<int> m_Positions;
        std::vector<int> m_Offsets;
        std::vector<int> m_TextureUV;
        std::vector<float> m_Colors;
        std::vector<unsigned int> m_Indices;

        GLuint m_PositionsBuffer = 0;
        GLuint m_OffsetsBuffer = 0;
        GLuint m_TextureUVBuffer = 0;
        GLuint m_ColorBuffer = 0;
        GLuint m_IndicesBuffer = 0;
        GLuint m_VBO = 0;
        bool m_BuffersDirty = false;
    };
}