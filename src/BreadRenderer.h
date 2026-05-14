#pragma once

#include <flecs/flecs.h>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

#include "Core/Rotation90.h"
#include "Rendering/RenderableResourceId.h"
#include "Rendering/RendererMap.h"
#include "Rendering/ShaderProgramMap.h"

typedef unsigned int GLuint;

namespace xc
{
    struct Font;

    class ShaderProgram;
}

namespace xg
{
    class UI;

    struct BreadRenderer;
    struct CogBoxRenderer;
    struct CogNodeRenderer;
    struct GridRenderer;
    struct TextRenderer;
    struct RenderSettings;

    struct BreadRenderer
    {
        BreadRenderer();
        ~BreadRenderer();

        void Load(const xg::RenderSettings& settings);

        void Update(const flecs::world& world);
        void Draw(const flecs::world& world);

    private:
        xg::ShaderProgramMap m_ShaderProgramMap;
        xg::CogRendererMap m_CogRendererMap;
        xg::CogRendererMap m_CogPreviewRendererMap;
        xg::CogRendererMap m_CogPreviewDropRendererMap;
        xg::WireRendererMap m_WireRendererMap;

        std::unique_ptr<xc::Font> m_Font;
        std::unique_ptr<xg::TextRenderer> m_TextRenderer;
        std::unique_ptr<xg::GridRenderer> m_GridRenderer;
        std::unique_ptr<xg::CogBoxRenderer> m_CogBoxRenderer;
        std::unique_ptr<xg::CogBoxRenderer> m_CogBoxPreviewRenderer;
        std::unique_ptr<xg::CogBoxRenderer> m_CogBoxPreviewDropRenderer;
        std::unique_ptr<xg::CogNodeRenderer> m_CogNodeRenderer;

        std::unique_ptr<xc::ShaderProgram> m_TextProgram;
        std::unique_ptr<xc::ShaderProgram> m_GridProgram;
        std::unique_ptr<xc::ShaderProgram> m_CogBoxProgram;
        std::unique_ptr<xc::ShaderProgram> m_CogNodeProgram;

        GLuint m_WireTexture = 0;
        glm::ivec2 m_WireTextureSize = glm::ivec2(0);

        float m_FontSize = 0.2f;
        glm::vec2 m_Position = glm::vec2(-0.9f, -0.9f);
        glm::vec3 m_Color = glm::vec3(1.f, 1.f, 1.f);
        std::string m_Text = "Hello world";
    };
}