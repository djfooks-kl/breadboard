#pragma once

#include <unordered_map>

#include "Core/ShaderProgram.h"
#include "ShaderProgramResourceId.h"

namespace xg
{
    using ShaderProgramMap = std::unordered_map<xg::ShaderProgramResourceId, xc::ShaderProgram>;
}