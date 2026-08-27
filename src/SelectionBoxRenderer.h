#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "SelectionBoxUniforms.h"
#include "Rendering/VertexBufferObject.h"

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct SelectionBoxRenderer
    {
        SelectionBoxRenderer(const xc::ShaderProgram& program);

        void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::vec2& p1,
            const glm::vec2& p2);

        bool HasBox() const;

        void AddBox();

        void RemoveAll();

        xg::SelectionBoxUniforms m_Uniforms;

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        xg::SelectionBoxUniformsLocations m_UniformsLocations;
    };
}