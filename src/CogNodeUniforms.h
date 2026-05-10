#pragma once

#include <glm/vec3.hpp>

typedef int GLint;

namespace xg
{
    struct CogNodeUniforms
    {
        glm::vec3 m_RingColor;
        float m_InnerRadius = 0.5f;
        float m_OuterRadius = 0.5f;
        float m_Radius = 1.f;
    };
    // -- [CODEGEN START] UniformsHeader(viewProjection, feather, wireTextureSize)
    struct CogNodeUniformsLocations
    {
        GLint m_RingColor = -1;
        GLint m_InnerRadius = -1;
        GLint m_OuterRadius = -1;
        GLint m_Radius = -1;
        GLint m_ViewProjection = -1;
        GLint m_Feather = -1;
        GLint m_WireTextureSize = -1;
    };
    // -- [CODEGEN END]
}
