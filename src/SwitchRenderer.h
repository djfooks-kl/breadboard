#pragma once

#include "Rendering/IRenderer.h"

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

typedef int GLint;
typedef unsigned int GLuint;

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    struct SwitchRenderer : public xg::IRenderer
    {
        SwitchRenderer(const xc::ShaderProgram& program);
        ~SwitchRenderer();

        void AddRenderable(
            const glm::ivec2& position,
            const xc::Rotation90 rotation,
            const int flags,
            const glm::ivec2& infoUV) override;

        void RemoveAll() override;

        void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::ivec2& infoTextureSize,
            const GLuint infoTexture) override;

        void SetColor(const glm::vec3 v) { m_Color = std::move(v); }
        void SetHasInfoTexture(const bool v) { m_HasInfoTexture = v; }

        void AddSwitch(
            const glm::ivec2& basePosition,
            const glm::ivec2& p1,
            const glm::ivec2& p2,
            const glm::ivec2& infoUV);

    private:
        const xc::ShaderProgram& m_Program;
        glm::vec3 m_Color;
        bool m_HasInfoTexture;
        float m_InnerRadius;
        float m_OuterRadius;
        float m_InnerContactWidth;
        float m_OuterContactWidth;

        std::vector<int> m_BaseP;
        std::vector<int> m_P1;
        std::vector<int> m_P2;
        std::vector<float> m_TextureUV;
        std::vector<int> m_WireUVs;
        std::vector<unsigned int> m_Indices;

        GLuint m_BasePBuffer = 0;
        GLuint m_P1Buffer = 0;
        GLuint m_P2Buffer = 0;
        GLuint m_TextureUVBuffer = 0;
        GLuint m_WireUVBuffer = 0;
        GLuint m_IndicesBuffer = 0;

        GLuint m_VBO = 0;
        bool m_BuffersDirty = false;

        GLint m_FeatherUniform = -1;
        GLint m_WireTextureSizeUniform = -1;
        GLint m_ViewProjectionUniform = -1;
        GLint m_ColorUniform = -1;
        GLint m_InnerRadiusUniform = -1;
        GLint m_OuterRadiusUniform = -1;
        GLint m_InnerContactWidthUniform = -1;
        GLint m_OuterContactWidthUniform = -1;
    };
}