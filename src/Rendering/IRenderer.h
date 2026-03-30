#pragma once

#include <glm/fwd.hpp>

#include "Core/Rotation90.h"

typedef unsigned int GLuint;

namespace xg
{
    struct IRenderer
    {
        virtual ~IRenderer() = default;

        virtual void AddRenderable(
            const glm::ivec2& position,
            const xc::Rotation90 rotation,
            const int flags,
            const glm::ivec2& infoUV) = 0;

        virtual void RemoveAll() = 0;

        virtual void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::ivec2& infoTextureSize,
            const GLuint infoTexture) = 0;

        virtual float GetHeight() { return 0.f; }
    };
}
