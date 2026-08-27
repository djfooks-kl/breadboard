#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

typedef int GLint;

namespace xg
{
    struct SelectionBoxUniforms
    {
        glm::vec3 m_EdgeColor = glm::vec3(0.f);
        glm::vec3 m_FillColor = glm::vec3(0.f);
        float m_FillAlpha = 0.f;
        float m_EdgeWidth = 0.f;
    };
    // -- [CODEGEN START] UniformsHeader(viewProjection, feather, min, max)
    struct SelectionBoxUniformsLocations
    {
        GLint m_EdgeColor = -1;
        GLint m_FillColor = -1;
        GLint m_FillAlpha = -1;
        GLint m_EdgeWidth = -1;
        GLint m_ViewProjection = -1;
        GLint m_Feather = -1;
        GLint m_Min = -1;
        GLint m_Max = -1;
    };
    // -- [CODEGEN END]
}
