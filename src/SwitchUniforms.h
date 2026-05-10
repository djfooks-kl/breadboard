#pragma once

#include <glm/vec3.hpp>

typedef int GLint;

namespace xg
{
    struct SwitchUniforms
    {
        glm::vec3 m_OutlineColor;
        bool m_HasInfoTexture = false;
        float m_InnerRadius = 0.26f;
        float m_OuterRadius = 0.31f;
        float m_InnerContactWidth = 0.07f;
        float m_OuterContactWidth = 0.14f;
    };
    // -- [CODEGEN START] UniformsHeader(viewProjection, feather, wireTextureSize)
    struct SwitchUniformsLocations
    {
        GLint m_OutlineColor = -1;
        GLint m_HasInfoTexture = -1;
        GLint m_InnerRadius = -1;
        GLint m_OuterRadius = -1;
        GLint m_InnerContactWidth = -1;
        GLint m_OuterContactWidth = -1;
        GLint m_ViewProjection = -1;
        GLint m_Feather = -1;
        GLint m_WireTextureSize = -1;
    };
    // -- [CODEGEN END]
}
