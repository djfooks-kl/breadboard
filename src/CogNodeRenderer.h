#pragma once

#include "Rendering/IRenderer.h"
#include "Rendering/VertexBufferObject.h"
#include "CogNodeUniforms.h"

#include <glm/fwd.hpp>

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

        void AddNode(
            const glm::ivec2& position,
            const glm::ivec2& wireUV);

        xg::CogNodeUniforms m_Uniforms;

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        xg::CogNodeUniformsLocations m_UniformsLocations;
    };
}