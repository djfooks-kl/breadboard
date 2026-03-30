#pragma once

#include <glm/fwd.hpp>

typedef unsigned int GLuint;

namespace xg
{
    struct IWireRenderer
    {
        virtual ~IWireRenderer() = default;

        virtual void AddWire(
            const glm::ivec2& /*p1*/,
            const glm::ivec2& /*p2*/,
            const glm::ivec2& /*infoUV*/)
        {
        }

        virtual void AddWireEnd(
            const glm::ivec2& /*position*/,
            const glm::ivec2& /*infoUV*/)
        {
        }

        virtual void RemoveAll() = 0;

        virtual void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::ivec2& infoTextureSize,
            const GLuint infoTexture) = 0;

        virtual float GetHeight() { return 0.f; }
    };
}
