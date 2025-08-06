#pragma once

#include <flecs/flecs.h>
#include <memory>

typedef unsigned int GLuint;

namespace bread::engine
{
    class ShaderProgram;
}

class Demo
{
public:
    Demo();
    ~Demo();

    void Init();
    void Update(double time, float deltaTime);

private:
    flecs::world m_World;

    std::unique_ptr<bread::engine::ShaderProgram> m_Program;
    GLuint m_VBO = 0;
    GLuint m_Texture = 0;

    bool m_ContextMenuOpen = false;
};