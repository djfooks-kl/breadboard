#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

typedef int GLint;
typedef unsigned int GLuint;

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct CogNodeRenderer
    {
        CogNodeRenderer(const xc::ShaderProgram& program);
        ~CogNodeRenderer();

        void SetRingColor(const glm::vec3 v) { m_RingColor = std::move(v); }
        void SetRadius(const float v) { m_Radius = v; }

        void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::ivec2& wireTextureSize,
            const GLuint texture);

        void AddNode(
            const glm::ivec2& position,
            const glm::ivec2& wireUV);

        void RemoveAllNodes();

    private:
        const xc::ShaderProgram& m_Program;
        glm::vec3 m_RingColor;
        float m_Radius;

        std::vector<int> m_Positions;
        std::vector<int> m_Circles;
        std::vector<int> m_WireUVs;
        std::vector<unsigned int> m_Indices;

        GLuint m_PositionBuffer = 0;
        GLuint m_CircleBuffer = 0;
        GLuint m_WireUVBuffer = 0;
        GLuint m_IndicesBuffer = 0;

        GLuint m_VBO = 0;
        bool m_BuffersDirty = false;

        GLint m_FeatherUniform = -1;
        GLint m_WireTextureSizeUniform = -1;
        GLint m_ViewProjectionUniform = -1;
        GLint m_RadiusUniform = -1;
        GLint m_RingColorUniform = -1;
    };
}