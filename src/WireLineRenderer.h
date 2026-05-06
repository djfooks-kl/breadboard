#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Rendering/VertexBufferObject.h"
#include "Rendering/IWireRenderer.h"

typedef unsigned int GLuint;

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct WireLineRenderer : public xg::IWireRenderer
    {
        WireLineRenderer(const xc::ShaderProgram& program);

#define ADD_UNIFORM(TYPE, NAME, DEFAULT_VALUE) void Set##NAME(const TYPE v) { m_##NAME = std::move(v); }
#include "WireLineRendererUniformList.h"
#undef ADD_UNIFORM

        float GetHeight() const override { return m_Height; }
        void SetHeight(const float v) { m_Height = v; }

        virtual void AddWire(
            const glm::ivec2& p1,
            const glm::ivec2& p2,
            const glm::ivec2& infoUV);

        void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::ivec2& infoTextureSize,
            const GLuint infoTexture);

        void RemoveAll();

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        float m_Height;

#define ADD_UNIFORM(TYPE, NAME, DEFAULT_VALUE) TYPE m_##NAME = TYPE(DEFAULT_VALUE);
#include "WireLineRendererUniformList.h"
#undef ADD_UNIFORM

#define ADD_UNIFORM(TYPE, NAME, DEFAULT_VALUE) GLint m_##NAME##Uniform = -1;
#include "WireLineRendererUniformList.h"
#undef ADD_UNIFORM

        GLint m_WireTextureSizeUniform = -1;
        GLint m_ViewProjectionUniform = -1;
        GLint m_FeatherUniform = -1;
    };
}