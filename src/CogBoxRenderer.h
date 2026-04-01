#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Rendering/VertexBufferObject.h"

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct CogBoxRenderer
    {
        CogBoxRenderer(const xc::ShaderProgram& program);

        void SetColor(const glm::vec3 v) { m_Color = std::move(v); }
        void SetFillColor(const glm::vec3 v) { m_FillColor = std::move(v); }

        void Draw(
            const glm::mat4& viewProjection,
            const float feather);

        void AddBox(
            const glm::ivec2& p1,
            const glm::ivec2& p2);

        void RemoveAll();

        float m_Border;
        float m_Expand;

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        glm::vec3 m_Color;
        glm::vec3 m_FillColor;

        GLint m_BorderUniform = -1;
        GLint m_ColorUniform = -1;
        GLint m_ExpandUniform = -1;
        GLint m_FeatherUniform = -1;
        GLint m_FillColorUniform = -1;
        GLint m_ViewProjectionUniform = -1;
    };
}