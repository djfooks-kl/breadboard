#pragma once

#include <glm/vec3.hpp>

typedef unsigned int GLuint;

namespace xg
{
    struct CogBoxUniforms
    {
        glm::vec3 m_Color = glm::vec3(0.f);
        glm::vec3 m_FillColor = glm::vec3(0.f);
        float m_Size = 0.f;
        float m_Expand = 0.f;
    };
    // -- [CODEGEN START] UniformsHeader(viewProjection, feather)
    struct CogBoxUniformsLocations
    {
        GLint m_Color = -1;
        GLint m_FillColor = -1;
        GLint m_Size = -1;
        GLint m_Expand = -1;
        GLint m_ViewProjection = -1;
        GLint m_Feather = -1;
    };
    // -- [CODEGEN END]
}
