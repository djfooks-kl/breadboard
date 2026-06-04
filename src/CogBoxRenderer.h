#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Rendering/VertexBufferObject.h"
#include "CogBoxUniforms.h"

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct CogBoxRenderer
    {
        CogBoxRenderer(const xc::ShaderProgram& program);

        void Draw(
            const glm::mat4& viewProjection,
            const float feather);

        void AddBox(
            const glm::ivec2& p1,
            const glm::ivec2& p2);

        void RemoveAll();

        xg::CogBoxUniforms m_Uniforms;

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        xg::CogBoxUniformsLocations m_UniformsLocations;
    };
}