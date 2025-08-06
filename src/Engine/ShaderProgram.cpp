#include "Engine/ShaderProgram.h"

#include <array>
#include <format>
#include <fstream>

#include "GLFWLib.h"
#include "Engine/ShaderHelpers.h"

bread::engine::ShaderProgram::ShaderProgram(ShaderProgramOptions options)
    : m_Options(std::move(options))
{
}

bread::core::Result<void> bread::engine::ShaderProgram::TryLoad()
{
    bread::core::Result<GLuint> vertexResult = bread::engine::CompileShader(GL_VERTEX_SHADER, std::format("{}{}", DATA_DIR, m_Options.m_VertexPath));
    if (!vertexResult)
    {
        return core::ResultError{ vertexResult.error() };
    }
    bread::core::Result<GLuint> fragmentResult = bread::engine::CompileShader(GL_FRAGMENT_SHADER, std::format("{}{}", DATA_DIR, m_Options.m_FragmentPath));
    if (!fragmentResult)
    {
        return core::ResultError{ fragmentResult.error() };
    }
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertexResult.value());
    glAttachShader(m_Program, fragmentResult.value());
    glLinkProgram(m_Program);
    GLint linked;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &linked);
    if (linked)
    {
        return {};
    }

    std::array<char, 4096> buffer;
    GLsizei length;
    glGetProgramInfoLog(m_Program, static_cast<GLsizei>(buffer.size()), &length, buffer.data());
    return core::ResultError{ buffer.data() };
}

void bread::engine::ShaderProgram::TryLoadAndOutputError()
{
    bread::core::Result<void> result = TryLoad();
    if (!result)
    {
        printf("%s\n", result.error().c_str());
    }
}