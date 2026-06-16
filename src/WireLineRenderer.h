#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Rendering/VertexBufferObject.h"
#include "Rendering/IWireRenderer.h"
#include "WireLineUniforms.h"

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct WireLineRenderer : public xg::IWireRenderer
    {
        WireLineRenderer(const xc::ShaderProgram& program);

        float GetHeight() const override { return m_Height; }
        void SetHeight(const float v) { m_Height = v; }

        void AddWire(
            const glm::ivec2& p1,
            const glm::ivec2& p2,
            const glm::ivec2& infoUV) override;

        void SetValid(bool valid) override;

        void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::ivec2& infoTextureSize,
            const GLuint infoTexture) override;

        void RemoveAll() override;

        xg::WireLineUniforms m_Uniforms;

        glm::vec3 m_ColorEdge = glm::vec3(0.f);
        glm::vec3 m_ColorInvalidEdge = glm::vec3(1.f, 0.f, 0.f);

    private:
        const xc::ShaderProgram& m_Program;
        xg::VertexBufferObject m_VBO;

        float m_Height = 0.f;
        bool m_Valid = true;

        xg::WireLineUniformsLocations m_UniformsLocations;
    };
}