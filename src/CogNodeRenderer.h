#pragma once

#include "Rendering/IRenderer.h"
#include "Rendering/VertexBufferObject.h"

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
    struct CogNodeRenderer : public xg::IRenderer
    {
        CogNodeRenderer(const xc::ShaderProgram& program);

        DECLARE_IRENDERER_FUNCTIONS();

        void SetRingColor(const glm::vec3 v) { m_RingColor = std::move(v); }
        void SetRadius(const float v) { m_Radius = v; }

        void AddNode(
            const glm::ivec2& position,
            const glm::ivec2& wireUV);

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        glm::vec3 m_RingColor;
        float m_Radius = 1.f;

        GLint m_FeatherUniform = -1;
        GLint m_WireTextureSizeUniform = -1;
        GLint m_ViewProjectionUniform = -1;
        GLint m_RadiusUniform = -1;
        GLint m_RingColorUniform = -1;
    };
}