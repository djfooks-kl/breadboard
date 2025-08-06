#pragma once

#include <string>

#include "Core/Result.h"

typedef unsigned int GLuint;

namespace bread::engine
{
    struct ShaderProgramOptions
    {
        std::string m_Name;
        std::string m_VertexPath;
        std::string m_FragmentPath;
    };

    class ShaderProgram
    {
    public:
        ShaderProgram(ShaderProgramOptions options);

        bread::core::Result<void> TryLoad();
        void TryLoadAndOutputError();

        GLuint GetProgramId() const { return m_Program; }

    private:
        ShaderProgramOptions m_Options;
        GLuint m_Program = 0;
    };
}