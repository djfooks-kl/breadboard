#pragma once

#include <flecs/flecs.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace xc
{
    class ShaderProgram;
}

namespace xg
{
    class UI;
    struct BreadRenderer;
}

typedef unsigned int GLuint;

struct BoxRenderer;
struct GLFWwindow;

class BreadApp
{
public:
    BreadApp();
    ~BreadApp();

    void Init(GLFWwindow* window);
    void Update(GLFWwindow* window, double time, float deltaTime);

    void Render(double time, float deltaTime);

    void ProcessKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ProcessCursorInput(GLFWwindow* window, double xpos, double ypos);

private:
    std::unique_ptr<xg::BreadRenderer> m_BreadRenderer;
    std::unique_ptr<BoxRenderer> m_BoxRenderer;
    std::unique_ptr<xg::UI> m_UI;
    flecs::world m_World;

    std::unique_ptr<xc::ShaderProgram> m_DemoProgram;
    std::unique_ptr<xc::ShaderProgram> m_BoxProgram;
    GLuint m_PositionsBuffer;
    GLuint m_TextureUBuffer;
    GLuint m_IndicesBuffer;
    GLuint m_DemoVBO;
    GLuint m_Texture;
};