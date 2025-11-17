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
    struct CogBoxRenderer
    {
        CogBoxRenderer(const xc::ShaderProgram& program);
        ~CogBoxRenderer();

        void SetColor(const glm::vec3 v) { m_Color = std::move(v); }
        void SetFillColor(const glm::vec3 v) { m_FillColor = std::move(v); }

        void Draw(
            const glm::mat4& viewProjection,
            const float feather);

        void AddBox(
            const glm::ivec2& p1,
            const glm::ivec2& p2);

        void RemoveAllBoxes();

        float m_Border;
        float m_Expand;

    private:
        const xc::ShaderProgram& m_Program;
        glm::vec3 m_Color;
        glm::vec3 m_FillColor;

        std::vector<int> m_P1;
        std::vector<int> m_P2;
        std::vector<float> m_TextureUV;
        std::vector<unsigned int> m_Indices;

        GLuint m_P1Buffer = 0;
        GLuint m_P2Buffer = 0;
        GLuint m_TextureUVBuffer = 0;
        GLuint m_IndicesBuffer = 0;
        GLuint m_VBO = 0;
        bool m_BuffersDirty = false;

        GLint m_BorderUniform = -1;
        GLint m_ColorUniform = -1;
        GLint m_ExpandUniform = -1;
        GLint m_FeatherUniform = -1;
        GLint m_FillColorUniform = -1;
        GLint m_ViewProjectionUniform = -1;
    };
}