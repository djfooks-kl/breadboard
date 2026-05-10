#pragma once

#include <glm/fwd.hpp>

typedef int GLint;

namespace xg
{
    void SetUniform(GLint uniform, bool value);
    void SetUniform(GLint uniform, float value);
    void SetUniform(GLint uniform, const glm::vec2& value);
    void SetUniform(GLint uniform, const glm::vec3& value);
    void SetUniform(GLint uniform, const glm::mat4& value);
}