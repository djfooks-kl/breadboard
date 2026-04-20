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

        void SetColorEmpty(const glm::vec3 v) { m_ColorEmpty = std::move(v); }
        void SetColorFull(const glm::vec3 v) { m_ColorFull = std::move(v); }
        void SetColorEdge(const glm::vec3 v) { m_ColorEdge = std::move(v); }
        void SetHasInfoTexture(const bool v) { m_HasInfoTexture = v; }
        void SetInnerWidth(const float v) { m_InnerWidth = v; }
        void SetOuterWidth(const float v) { m_OuterWidth = v; }
        void SetExpand(const bool v) { m_Expand = v; }

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

        glm::vec3 m_ColorEmpty;
        glm::vec3 m_ColorFull;
        glm::vec3 m_ColorEdge;
        bool m_HasInfoTexture = false;
        bool m_Expand = false;
        float m_Height = 0.f;
        float m_InnerWidth = 0.03f;
        float m_OuterWidth = 0.07f;

        GLint m_ColorEmptyUniform = -1;
        GLint m_ColorFullUniform = -1;
        GLint m_ColorEdgeUniform = -1;
        GLint m_WireTextureSizeUniform = -1;
        GLint m_ViewProjectionUniform = -1;
        GLint m_FeatherUniform = -1;
        GLint m_InnerWidthUniform = -1;
        GLint m_OuterWidthUniform = -1;
        GLint m_ExpandUniform = -1;
    };
}