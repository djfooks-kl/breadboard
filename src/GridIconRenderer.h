#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/Rotation90.h"
#include "Rendering/IRenderer.h"
#include "Rendering/VertexBufferObject.h"

typedef int GLint;
typedef unsigned int GLuint;

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct GridIconRenderer : public xg::IRenderer
    {
        GridIconRenderer(const xc::ShaderProgram& program);

        DECLARE_IRENDERER_FUNCTIONS();

        void SetIconSize(const float v) { m_IconSize = v; }
        void SetColor(const glm::vec3& v) { m_Color = v; }

        void AddIcon(
            const glm::ivec2& position,
            const xc::Rotation90 rotation,
            const glm::vec3& color);

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        float m_IconSize = 1.f;
        glm::vec3 m_Color = glm::vec3(0.f);

        GLint m_ViewProjectionUniform = -1;
        GLint m_FeatherUniform = -1;
        GLint m_IconSizeUniform = -1;
        GLint m_ColorUniform = -1;
    };
}