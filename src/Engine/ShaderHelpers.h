#pragma once

#include <string>

#include "Core/Result.h"

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace bread::engine
{
	bread::core::Result<GLuint> CompileShader(const GLenum type, const std::string& path);
}