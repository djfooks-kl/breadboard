#pragma once

#include "Rendering/IRenderer.h"
#include "Rendering/VertexBufferObject.h"
#include "SwitchUniforms.h"

#include <glm/fwd.hpp>

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct SwitchRenderer : public xg::IRenderer
    {
        SwitchRenderer(const xc::ShaderProgram& program);

        DECLARE_IRENDERER_FUNCTIONS();

        void AddSwitch(
            const glm::ivec2& basePosition,
            const glm::ivec2& p1,
            const glm::ivec2& p2,
            const glm::ivec2& infoUV);

        xg::SwitchUniforms m_Uniforms;

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        xg::SwitchUniformsLocations m_UniformsLocations;
    };
}