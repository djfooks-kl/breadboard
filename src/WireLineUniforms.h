#pragma once

#include <glm/vec3.hpp>

typedef unsigned int GLuint;

namespace xg
{
    struct WireLineUniforms
    {
        glm::vec3 m_ColorEmpty;
        glm::vec3 m_ColorFull;
        glm::vec3 m_ColorEdge;
        bool m_HasInfoTexture = false;
        bool m_Expand = false;
        float m_InnerWidth = 0.03f;
        float m_OuterWidth = 0.07f;
    };
    // -- [CODEGEN START] UniformsHeader(viewProjection, feather, wireTextureSize)
    struct WireLineUniformsLocations
    {
        GLint m_ColorEmpty = -1;
        GLint m_ColorFull = -1;
        GLint m_ColorEdge = -1;
        GLint m_HasInfoTexture = -1;
        GLint m_Expand = -1;
        GLint m_InnerWidth = -1;
        GLint m_OuterWidth = -1;
        GLint m_ViewProjection = -1;
        GLint m_Feather = -1;
        GLint m_WireTextureSize = -1;
    };
    // -- [CODEGEN END]
}
