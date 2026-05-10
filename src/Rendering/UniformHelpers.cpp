#include "UniformHelpers.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/GLFWLib.h"

void xg::SetUniform(GLint uniform, bool value)
{
    glUniform1f(uniform, value ? 1.f : 0.f);
}

void xg::SetUniform(GLint uniform, float value)
{
    glUniform1f(uniform, value);
}

void xg::SetUniform(GLint uniform, const glm::vec2& value)
{
    glUniform2fv(uniform, 1, glm::value_ptr(value));
}

void xg::SetUniform(GLint uniform, const glm::vec3& value)
{
    glUniform3fv(uniform, 1, glm::value_ptr(value));
}

void xg::SetUniform(GLint uniform, const glm::mat4& value)
{
    glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(value));
}