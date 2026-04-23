#pragma once

#include <glm/fwd.hpp>

#include "Core/ITransform.h"

typedef unsigned int GLuint;

namespace xg
{
    struct IRenderer
    {
        virtual ~IRenderer() = default;

        virtual void AddRenderable(
            const xc::ITransform& transform,
            const int flags,
            const glm::ivec2& infoUV) = 0;

        virtual void RemoveAll() = 0;

        virtual void Draw(
            const glm::mat4& viewProjection,
            const float feather,
            const glm::ivec2& infoTextureSize,
            const GLuint infoTexture) = 0;

        virtual float GetHeight() const { return 0.f; }
    };
}

#define DECLARE_IRENDERER_FUNCTIONS()          \
    void AddRenderable(                        \
        const xc::ITransform& transform,       \
        const int flags,                       \
        const glm::ivec2& infoUV) override;    \
                                               \
    void RemoveAll() override;                 \
                                               \
    void Draw(                                 \
        const glm::mat4& viewProjection,       \
        const float feather,                   \
        const glm::ivec2& infoTextureSize,     \
        const GLuint infoTexture) override;