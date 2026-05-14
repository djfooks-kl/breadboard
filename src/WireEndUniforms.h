#pragma once

#include <glm/vec3.hpp>

typedef unsigned int GLuint;

namespace xg
{
    struct WireEndUniforms
    {
        glm::vec3 m_ColorEmpty = glm::vec3(0.f);
        glm::vec3 m_ColorFull = glm::vec3(0.f);
        float m_Size = 1.f;
        bool m_HasInfoTexture = false;
        bool m_Expand = false;
    };
    // -- [CODEGEN START] UniformsHeader(viewProjection, feather, wireTextureSize)
    struct WireEndUniformsLocations
    {
        GLint m_ColorEmpty = -1;
        GLint m_ColorFull = -1;
        GLint m_Size = -1;
        GLint m_HasInfoTexture = -1;
        GLint m_Expand = -1;
        GLint m_ViewProjection = -1;
        GLint m_Feather = -1;
        GLint m_WireTextureSize = -1;
    };
    // -- [CODEGEN END]
}
